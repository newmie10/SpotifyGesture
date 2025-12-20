# Handnet Planning

## Data Collection
- `Data_Collection.cpp`
- Collect data in native 160x120 color format
- Mounted in production location
- Started with black construction paper as background
    - Transitioned to black painted wood framing for more consistency
- Used autoclicker to take ~3 pictures per second
- Varied hand slightly to account for different hover levels, rotations, angles
- Added shading to account for shadows during demos

### Preprocessing
- Added color augmenting to assist inference on darked skin (training only)
- Crop images from 160x120 to 106x94
- Then rescale from 106x94 to 48x48
- Convert to grayscale 

### Testing
- Test model on 20% test set
- Test model on rotated test set
- Tested model on ~15% gaussian noise test set

## Speed Testing
- `Inference_Testing.cpp`
- Profiled code into all different processes
    - Camera Capture
    - Image Preprocessing
    - Fill Input Tensor
    - Run Model Inference
    - Process Output
    - Results Summary
- ***Include example outputs***
- Allowed for comparision of models with inputs and hidden layers sizes scaled based on the original 64x64 architecture
- Ultimately concluded 48x48 was a good middle ground of inference time and predective performance

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

## Phase 3 

### Data Collection

#### Phase 3

- Done in CS Makerspace (Similar to production environment)
- Located in Data_Collection.cpp

### Gestures

- Open Hand: Play
- Closed Hand: Pause
- None: Do nothing (classify accidental trigger and noise)

### Challenges
- Inference takes ~5 seconds with model scale and input size
- Need to scale from 64x64 to 32x32 or 48x48 to accelerate model inference

## Final Model
- Done in LGRC A104A to get real production lighting
- Used same file in Data_Collection.,cpp

### Gestures

- Open Hand: Play
- Closed Hand: Pause
- None: Do nothing (classify accidental trigger and noise)

### Model Optimizations
- Quanitize Model to int8 from fp32 (4x reduction)
- Change input size from 64x64 (~5 sec inference) to 48x48 (~1.5 sec inference)

## Triggering Camera
- Initial idea: Use distance sensor facing down to trigger
- 3D printed model broke, as well as we ran out of serial communication ports

### Mean Filter
- Used mean greyscale pixel value of the entire picture
- When elevated above threshold, camera triggers

### Signal Debouncing / Filtering
- 200 ms after initial trigger, check again to ensure mean is high enough before trigger
- Accounts for swiping noise from gesturing in front of camera
- After 200 ms, run inference using handnet

### Communication with spotify
- Based on inference, print final result to serial
- Python serial monitor checks for phrasing and communicates with spotify API