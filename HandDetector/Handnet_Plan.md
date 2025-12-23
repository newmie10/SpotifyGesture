# Handnet Planning

## Camera Testing
- Simple web app to recieve stream
- Tested OV Cam functionality, header file, pin setup, etc.

## Data Collection
- `Data_Collection.cpp`
- Collect data in native 160x120 color format
- Mounted in production location
- Started with black construction paper as background
    - Transitioned to black painted wood framing for more consistency
- Used autoclicker to take ~3 pictures per second
- Varied hand slightly to account for different hover levels, rotations, angles
- Added shading to account for shadows during demos
- Named images on data collection web app, iterated through count for numbers

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

### Challenges
- Images were too noisy
- Basically realized we had to focus the camera down at some sort of blank background
- ESP32 size constraints mean we needed a smaller model than possible for the capacity this dataset neededd

## Phase 2: Adjust to simplify model

- Hand signals were not different enough - some looked very similar after preprocessing

### New Gestures

- Peace Sign: Like Song
- Open Hand: Play
- Closed Hand: Pause
- ~500 each

### Challenges
- Peace still inferred as open hand, two fingers not disguisable

## Phase 3 

### Data Collection

- Done in CS Makerspace (Similar to production environment)
- Located in Data_Collection.cpp
- ~500 each

### Gestures

- Open Hand: Play
- Closed Hand: Pause
- None: Do nothing (classify accidental trigger and noise)

### Challenges
- Inference takes ~5 seconds with model scale and input size
- Need to scale input and hidden layers from 64x64 to 32x32 or 48x48 to accelerate model inference

## Final Model
- Done in LGRC A104A to get real production lighting
- Used same file in Data_Collection.cpp
- ~1000 each class

### Gestures

- Open Hand: Play
- Closed Hand: Pause
- None: Do nothing (classify accidental trigger and noise)

### Model Optimizations
- Quanitize Model to int8 from fp32 (4x reduction)
- Change input size and hidden layers from 64x64 scale (~5 sec inference) to 48x48 scale (~1.5 sec inference)

## Triggering Capture
- Initial idea: Use distance sensor facing down to trigger
- 3D printed model broke, as well as we ran out of serial communication ports

### Mean Filter
- Used mean greyscale pixel value of the entire picture
- When elevated above threshold, capture signal is given 

### Signal Debouncing / Filtering
- 200 ms after initial trigger, check again to ensure mean fitler is still active before trigger
    - Accounts for swiping noise from gesturing intended for distance sensor in front of camera
- After 200 ms, run inference using handnet

## Communication with spotify
- `main.cpp` / `SpotifyLink.cpp`
- Simplify final output and serial loggings
- Based on inference, print final result to serial
- Python serial monitor checks for phrasing and communicates with spotify API, noted in other documentation


## Full Story

- First setup camera and all - had trouble with psram on board but got it to stream to the web
- Insert video here
- Then started doing data collection by saving pngs on the web, sending the buffer through wifi to my computer. Connected to the esp32s network for data collection
- Realized that phase 1 images (insert pictures) were too noisy. instead needed a blank background

- Transitioned to build a downward facing view for the camera via the apparatus
- went to makerspace, built something that would allow us to have the esp32 facing down at a base
- Attached a black piece of construction paper to the bottom for a dark constant background

- Tested height, after adjusting drilled a hole through and mounted camera
- Tried phase 2 with 3 classes, peace open hand and closed hand (insert images)
- Realized open hand and peace looked too similar for the small cnn, even 64x64 model could not distinguish it

- Moved to phase 3, adjusted to open hand and closed hand, but then had a third class of nothing to denoise
- Insert images
- Seemed to work well although the nothing class was still getting mixed up 
- Had a test set accuracy of 100% but could not deal well with noise and such. could be overfit

- Moved on to model speed testing
- Realized inference from the 64x64 scale model took around ~5 seconds
- 32x32 only took half a second, but was not very accurate
- settled on ~1.5 second inference from data scaled to 48x48 with hidden layers scaled there
- Used printing and profiling to ensure speed

- Finally collected data on the final black painted mount in the real production room
- Took different hand angles and rotations, as well as different shadings using my computer case to capture variation
- ~1000 images each to increase the training data and help capture variance
- Then augmented images to be darker to help with darker skin inference
- worked very well, and even 80%+ accuracy on images with 5% gaussian noise, and perfect on rotated images.