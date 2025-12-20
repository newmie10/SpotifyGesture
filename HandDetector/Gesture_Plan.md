# Gesture Planning

## Phase 1: Planned Outline

### Gestures

- Thumbs up: Like song
- Thumbs down: Unlike Song
- Open Hand: Stop
- Closed hand: Play
- Point right: Skip
- Point left: Rewind

### Pipeline 

1. Button click -> take snapshot
2. 160x120: Lowest Resolution ingestion OV2640 cam
3. Real pipeline: Use PIXFORMAT_GRAYSCALE, Training: convert to greyscale
4. Downscale and crop to 64x64 through interpolation and pooling
5. Use NN classifier (either raw or pretrained) to classify between the gestures (no 'lack of gesture' yet)
6. Display through serial

## Phase 2: Adjust to simplify model

- Hand signals were not different enough - some looked very similar

### Gestures

- Peace Sign: Like Song
- Open Hand: Play
- Closed Hand: Pause

### Challenges
- Peace still infered as open hand

## Phase 3 / Final Classification

### Data Collection

#### Phase 3

- Done in CS Makerspace (Similar to production environment)
- Located in Data_Collection.cpp

#### Phase 4
- Done in LGRC A104A to get real production lighting
- Used same file in Data_Collection.,cpp

### Gestures

- Open Hand: Play
- Closed Hand: Pause
- None: Do nothing (classify accidental trigger and noise)

## Triggering Camera
- Initial idea: Use distance sensor facing down to trigger
- 3D printed model broke, as well as we ran out of serial communication ports

### Mean Filter
- Used mean greyscale pixel value of the entire picture
- When elevated above threshold, camera triggers

### Signal Debouncing 
- 200 ms after initial trigger, check again to ensure mean is high enough before trigger
- Accounts for swiping noise from gesturing in front of camera
- After 200 ms, run inference using handnet

### Communication with spotify

- Based on inference, print final result to serial
- Python serial monitor checks for phrasing and communicates with spotify API