#include <windows.h>
#include <mmsystem.h>
#include <stdexcept>
#include <map>
#include <string>
#include <fstream>

void writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size)
{
    WAVEHDR header;
/*
* initialise the block header with the size
* and pointer.
*/
    ZeroMemory(&header, sizeof(WAVEHDR));
    header.dwBufferLength = size;
    header.lpData = block;
/*
* prepare the block for playback
*/
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
/*
* write the block to the device. waveOutWrite returns immediately
* unless a synchronous driver is used (not often).
*/
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    Sleep(1000);
    waveOutReset(hWaveOut);
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
/*
* wait a while for the block to play then start trying
* to unprepare the header. this will fail until the block has
* played.
*/
    Sleep(500);
    while(waveOutUnprepareHeader(hWaveOut,&header,sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)
    Sleep(1000);
    waveOutReset(hWaveOut);
}

LPSTR loadAudioBlock(const char* filename, DWORD* blockSize)
{
    HANDLE hFile= INVALID_HANDLE_VALUE;
    DWORD size = 0;
    DWORD readBytes = 0;
    void* block = NULL;
/*
* open the file
*/
    if((hFile = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
        )) == INVALID_HANDLE_VALUE)
        return NULL;
/*
* get it's size, allocate memory and read the file
* into memory. don't use this on large files!
*/
    do {
        if((size = GetFileSize(hFile, NULL)) == 0) 
            break;
        if((block = HeapAlloc(GetProcessHeap(), 0, size)) == NULL)
            break;
        ReadFile(hFile, block, size, &readBytes, NULL);
    } while(0);
    CloseHandle(hFile);
    *blockSize = size;
    return (LPSTR)block;
}

HWAVEOUT WavePlayDivice(DWORD nSample, WORD bps, WORD nChannel){
    HWAVEOUT hWaveOut; /* device handle */
    WAVEFORMATEX wfx; /* look this up in your documentation */
    MMRESULT result;/* for waveOut return values */
    /*
* first we need to set up the WAVEFORMATEX structure. 
* the structure describes the format of the audio.
*/
    wfx.nSamplesPerSec = nSample; /* sample rate */
    wfx.wBitsPerSample = bps; /* sample size */
    wfx.nChannels = nChannel; /* channels*/
/*
* WAVEFORMATEX also has other fields which need filling.
* as long as the three fields above are filled this should
* work for any PCM (pulse code modulation) format.
*/
    wfx.cbSize = 0; /* size of _extra_ info */
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
/*
* try to open the default wave device. WAVE_MAPPER is
* a constant defined in mmsystem.h, it always points to the
* default wave device on the system (some people have 2 or
* more sound cards).
*/
    if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        fprintf(stderr, "unable to open WAVE_MAPPER device\n");
        ExitProcess(1);
    }
    return hWaveOut;
}

class Note{
    public:
        Note(){}
        ~Note(){
            waveOutUnprepareHeader(_hWaveOut,&_header,sizeof(WAVEHDR));
            waveOutClose(_hWaveOut);
        }
        
        void NoteInit(DWORD nSample, WORD bps, WORD nChannel, const char* filename, char keyIndex){
            WAVEFORMATEX wfx; /* look this up in your documentation */
            MMRESULT result;/* for waveOut return values */
            DWORD blockSize;
            
            wfx.nSamplesPerSec = nSample; /* sample rate */
            wfx.wBitsPerSample = bps; /* sample size */
            wfx.nChannels = nChannel; /* channels*/
            wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
            wfx.cbSize = 0; /* size of _extra_ info */
            wfx.wFormatTag = WAVE_FORMAT_PCM;
            wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
            wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
            
            if(waveOutOpen(&_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
                fprintf(stderr, "unable to open WAVE_MAPPER device\n");
                ExitProcess(1);
            }
            _pressed=false;
            _block = loadAudioBlock(filename, &blockSize);
            
            ZeroMemory(&_header, sizeof(WAVEHDR));
            _header.dwBufferLength = blockSize;
            _header.lpData = _block;
            waveOutPrepareHeader(_hWaveOut, &_header, sizeof(WAVEHDR));
            
            _keyIndex=keyIndex;
        }
        
        bool play(){
            if(!_pressed){
                waveOutWrite(_hWaveOut, &_header, sizeof(WAVEHDR));            
                _pressed=true;
				return true;
            }
			else
				return false;
        }
        
        bool onlineplay(){
            if(!_pressed){
                //waveOutWrite(_hWaveOut, &_header, sizeof(WAVEHDR));            
                _pressed=true;
				return true;
            }
			else
				return false;
        }
        
        bool reset(){
            waveOutReset(_hWaveOut);
            _pressed=false;
			return true;
        }
        
        bool onlinereset(){
            //waveOutReset(_hWaveOut);
            _pressed=false;
			return true;
        }
    private:
        char _keyIndex;
        //string noteIndex;
        
        HWAVEOUT _hWaveOut;  //wave playing device
        LPSTR _block;        //note wave content
        bool _pressed;
        WAVEHDR _header;
};

class Instrument{
    public:
        Instrument(const char *filename, DWORD nSample, WORD bps, WORD nChannel){
            
            std::ifstream infile(filename, std::ifstream::in);
            int nNote;
            char notechar;
            std::string notewave;
            std::map<char,Note>::iterator it;
            
            basicset(nSample, bps, nChannel);
            infile>>nNote;
            _notemap.clear();
			
            for(int i=0;i<nNote;i++){
                infile>>notechar;
                infile>>notewave;
                Note tempNote;
                _notemap[notechar]=tempNote;
                _notemap[notechar].NoteInit(nSample, bps, nChannel, notewave.c_str(), notechar);
            }

        }
        ~Instrument(){}
        void basicset(DWORD nSample, WORD bps, WORD nChannel){ _nSample=nSample; _bps=bps; _nChannel=nChannel; }
        bool playnote(char notechar){
			if(_notemap.find(notechar)!=_notemap.end())
				return _notemap[notechar].play();
			else
				return false;
        }
        bool onlineplaynote(char notechar){  //not truly playing
			if(_notemap.find(notechar)!=_notemap.end())
				return _notemap[notechar].onlineplay();
			else
				return false;
        }
        bool resetnote(char notechar){ 
			if(_notemap.find(notechar)!=_notemap.end())
				_notemap[notechar].reset();
			else
				return false;
        }
        bool onlineresetnote(char notechar){ //not truly playing
			if(_notemap.find(notechar)!=_notemap.end())
				return _notemap[notechar].onlinereset();
			else
				return false;
        }
    private:
        std::map<char,Note> _notemap;
        DWORD _nSample;
        WORD _bps;
        WORD _nChannel;
};
