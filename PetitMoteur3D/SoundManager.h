#pragma once
#include "Manager.h"
#include <xaudio2.h>
#include <memory>
#include <map>
#include <string>


#pragma region preparation_analyse_audio

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#pragma endregion

class SongError {};

class SoundManager : public Manager<SoundManager>
{
    IXAudio2* mpXAudio2;
    IXAudio2MasteringVoice* mpMasterVoice;
    std::map<std::string, IXAudio2SourceVoice*> mSongMap;
    std::map<std::string, bool> mIsPlaying;

    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

public:

    //Constructeurs / destructeurs
    SoundManager();
    ~SoundManager();
    //Override

	void Start() override;
	void Update() override;
	void Close() override;

    //methodes
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="path"></param>
    /// <param name="voiceName"></param>
    /// <param name="loop">Nombre de fois ou on joue le son</param>
    /// <param name="volume"></param>
    void createMySourceVoice(const char* path, std::string voiceName, int loop, float volume);
    bool isPlaying(std::string voiceName);
    void setIsPlaying(std::string voiceName, bool actual);

    void playSong(const char* path, std::string voiceName, int loop = 0, float volume = 1.0);
    void stopSong(std::string voiceName);
    void resumeSong(std::string voiceName);
    void exitLoopSong(std::string voiceName);

    void stopAllSong();
    void resumeAllSong();
};
