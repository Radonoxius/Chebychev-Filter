/**
  * audio.h
  * Copyright 2016 ARM Ltd.
  */

#ifndef audio_H
#define audio_H

#define ARM_MATH_CM4

#include "stdint.h"

typedef enum {
    AUDIO_CH_LEFT  = 1,
    AUDIO_CH_RIGHT = 2,
    AUDIO_CH_BOTH  = (AUDIO_CH_LEFT | AUDIO_CH_RIGHT)
} DspAudioChannelSelect;
#define IS_LEFT_CH_SELECT(MASK)  (((MASK) & AUDIO_CH_LEFT) != 0)
#define IS_RIGHT_CH_SELECT(MASK) (((MASK) & AUDIO_CH_RIGHT) != 0)

typedef struct{
    int32_t mask;
    int32_t input_l;
    int32_t input_r;
    int32_t output_l;
    int32_t output_r;
} I2S_Data_TypeDef;

void ProcessData(I2S_Data_TypeDef* I2S_Data);

/*! This enum describes the different sampling frequency setup of the CODEC */
typedef enum {
    FS_8000_HZ  = 8000,
    FS_16000_HZ = 16000,
    FS_22050_HZ = 22050,
    FS_32000_HZ = 32000,
    FS_44100_HZ = 44100,
    FS_48000_HZ = 48000,
    FS_96000_HZ = 96000
} sampling_rate;

/*! This enum describes the different input setup of the CODEC */		
typedef enum {
    AUDIO_INPUT_LINE,  // Select ADC input Line In
    AUDIO_INPUT_MIC,       // Select ADC input Microphone In
} audio_input;

/*! This enum describes the different input setup of the CODEC */		
typedef enum {
    IO_METHOD_INTR,     // User I2S interruptions to controll the flow of the program (generate interruption when  irq_depth=<FIFO level)
    IO_METHOD_DMA,      // Use DMA requests to controll the flow of the program (generat a request whem dma_depth=<FIFO level)
    IO_METHOD_POLL,
} mode;

/*! This enum describes two channels of DMA data */		
typedef enum {
    LEFT = 0,
    RIGHT = 1,
} audio_data_channel;

/*! This enum describes the test pin */
typedef enum {
    PD15,
} test_pin_num;

#define TEST_PIN PD15

#define PING 1
#define PONG 0

#define HIGH 1
#define LOW 0

#define DMA_BUFFER_SIZE 256

short prbs(void);
void AudioInit(sampling_rate, audio_input, mode);

void TestPinInit(test_pin_num);
void TestPinOut(test_pin_num, int value);
void TestPinToggle(test_pin_num);

#endif /* audio_H */
