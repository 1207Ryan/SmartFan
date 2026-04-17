#ifndef __VOICE_RECOGNITION_H
#define __VOICE_RECOGNITION_H

void Voice_Recognition_Init(void);
void Voice_Recognition(void);
void Distance_Warn(void);
void Count_Down_Over(void);
void Voice_Fan_On(void);
void Voice_Fan_Off(void);
void Voice_Fan_Gear_Up(void);
void Voice_Fan_Gear_Max(void);
void Voice_Fan_Gear_Down(void);
void Voice_SetTempThreshold(void);
void Voice_Volume_Up(void);
void Voice_Volume_Max(void);
void Voice_Volume_Down(void);
void Voice_Volume_Min(void);
void Voice_Volume_Set(uint8_t Volume);
void Voice_Music_Play(void);
void Voice_Music_Pause(void);
void Voice_Music_Next(void);
void Voice_Music_Previous(void);
void Voice_Music_SetVolume(uint8_t MusicVolume);
void Voice_Music_SingleCycle(void);
void Voice_Music_SequentialPlay(void);
void Voice_Music_ShufflePlay(void);
void Voice_Music_PlayJayChou(void);
void Voice_Music_PlayJJ(void);
void Voice_Music_PlayLeehom(void);

#endif
