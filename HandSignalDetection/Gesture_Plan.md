# Gesture Planning

## Phase 1 Outline

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