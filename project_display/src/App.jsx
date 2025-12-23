import { useState } from 'react'
import ModelViewer from './components/ModelViewer'
import './App.css'

function EngineeringProcess() {
  const [selectedStep, setSelectedStep] = useState('concept')
  
  const steps = [
    { id: 'concept', title: 'Concept & Planning', icon: '💡' },
    { id: 'hardware', title: 'Hardware Design', icon: '🔧' },
    { id: 'handdetection', title: 'Hand Detection Model', icon: '✋' },
    { id: 'gesturedetection', title: 'Gesture Detection', icon: '👋' },
    { id: 'integration', title: 'System Integration', icon: '🔗' },
  ]
  
  return (
    <div className="engineering-container">
      <div className="flowchart">
        {steps.map((step, index) => (
          <div key={step.id} className="flowchart-item">
            <button
              className={`step-button ${selectedStep === step.id ? 'active' : ''}`}
              onClick={() => setSelectedStep(step.id)}
            >
              <span className="step-icon">{step.icon}</span>
              <span className="step-title">{step.title}</span>
              <span className="step-number">{index + 1}</span>
            </button>
            {index < steps.length - 1 && (
              <div className="flow-arrow">→</div>
            )}
          </div>
        ))}
      </div>
      
      <div className="process-content">
        {selectedStep === 'concept' && (
          <div className="step-details">
            <div className="step-header">
              <h3>💡 Concept & Planning</h3>
              <div className="team-attribution">Team: All Members</div>
            </div>
            
            <div className="concept-intro">
              <p>
                Spotify Gesture is an innovative solution for Spotify music control in environments where you need to focus on other things, like driving or studying. It takes advantage of laser-distance sensors for gesture detection that lets you swipe side to side to skip, rewind, or go back to the previous song.
              </p>
            </div>

            <div className="feature-grid">
              <div className="feature-card">
                <h4>Gesture Control</h4>
                <p>Swipe-based navigation using laser-distance sensors. Skip tracks, rewind, or go back with simple side-to-side hand movements.</p>
              </div>
              
              <div className="feature-card">
                <h4>Volume Mode</h4>
                <p>Quick volume tuning during playback via distance sensors. Move your hand closer or farther to adjust volume on the fly.</p>
              </div>
              
              <div className="feature-card">
                <h4>Hand Detection</h4>
                <p>CNN-based hand position recognition for deeper controls. Open palm to play, closed fist to pause. Fully customizable to your preferences.</p>
              </div>
              
              <div className="feature-card">
                <h4>OLED Display</h4>
                <p>Real-time feedback showing current track info and last executed command. Always know what's playing and what gesture was detected.</p>
              </div>
            </div>

            <div className="system-overview">
              <h4>Complete System</h4>
              <p>
                Everything comes together in a single-box solution with an OLED screen to display what songs are playing and visual feedback for executed commands. A buzzer signals when commands are recognized. Just plug it into your computer and you're ready to go. No additional setup required.
              </p>
              
              <div className="image-placeholder">
                <div className="placeholder-box">
                  <span>📦 System Diagram</span>
                  <p>Sensor → ESP32 → Computer → Spotify</p>
                </div>
              </div>
            </div>

                <div className="vision-section">
              <h4>The Vision</h4>
              <p>
                The possibilities are endless with fine-tuning capabilities for a multitude of different signals. We wanted to create something that makes music control intuitive and hands-free, perfect for situations where traditional controls are inconvenient or unsafe.
              </p>
            </div>

            <div className="specs-card">
              <h4>Technical Specifications</h4>
              <div className="specs-grid">
                <div className="spec-item">
                  <span className="spec-label">Model Accuracy (Test Set)</span>
                  <span className="spec-value">100%</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Real-Time Accuracy</span>
                  <span className="spec-value">100%</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Noise Robustness (5% Gaussian)</span>
                  <span className="spec-value">80%</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Rotated Images</span>
                  <span className="spec-value">100%</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Inference Time</span>
                  <span className="spec-value">~1.5s</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">System Latency</span>
                  <span className="spec-value">&lt;500ms</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Input Resolution</span>
                  <span className="spec-value">48×48</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">Model Precision</span>
                  <span className="spec-value">int8</span>
                </div>
                <div className="spec-item">
                  <span className="spec-label">ToF Detection Range</span>
                  <span className="spec-value">30cm</span>
                </div>
              </div>
              <p className="specs-note">
                <strong>Note:</strong> Main issues in real-world use came from premature inference triggering or misuse, not model accuracy limitations.
              </p>
            </div>
          </div>
        )}
        
        {selectedStep === 'hardware' && (
          <div className="step-details">
            <div className="step-header">
              <h3>🔧 Hardware Design</h3>
              <div className="team-attribution">Team: All Members</div>
            </div>
            
            <div className="hardware-intro">
              <p>
                Based on our preliminary investigation (detailed in the Hand Detection Model tab), we discovered that images with noisy backgrounds were extremely difficult to denoise and run inference on. This led to a key design decision: point the camera and sensors downward to establish an easier baseline state for both sensor and camera readings.
              </p>
            </div>

            <div className="hardware-section">
              <h4>First Prototype: Wood Platform</h4>
              <p>
                We headed to the all-campus makerspace to build a test platform for mounting the camera and sensors. Using scrap wood pieces and a drill, we constructed two vertical poles (one around 16 inches tall and another around 9 inches) and screwed them to a base board. On top of the taller pole, we mounted a small half-inch thick plank where we could attach the ESP32 and test the camera's field of view.
              </p>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HardwareDesign/Initial_Wood/Construct_1.png" alt="Apparatus Construction 1" />
                  <p>Apparatus Construction 1</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/Initial_Wood/Construct_2.png" alt="Apparatus Construction 2" />
                  <p>Apparatus Construction 2</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/Tape_Test/Tape_Test.png" alt="ESP32 Taped to Mount" />
                  <p>ESP32 Taped to Mount</p>
                </div>
              </div>
              
              <p>
                After several tests, we determined that around 14 inches would be the optimal height for the camera. We also found a piece of black construction paper to use as a dark background for training images.
              </p>
            </div>

            <div className="hardware-section">
              <h4>3D Printed Mount Attempt</h4>
              <p>
                Next, we tried 3D printing a mount for the ESP32 and ToF sensors. Austin designed a two-part mounting system, but the scale of the apparatus proved too large, so we decided to only print the top portion. While it seemed promising at first, we quickly realized mounting would be difficult and we needed more adjustability for the camera. Without enough time to prototype multiple iterations, we took a different approach.
              </p>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HardwareDesign/3d_Mount/Bottom_CAD.png" alt="CAD Design Bottom View" />
                  <p>CAD Design Bottom View</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/3d_Mount/Full_CAD.png" alt="CAD Design Full View" />
                  <p>CAD Design Full View</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/3d_Mount/Final_Print.png" alt="Final 3D Print" />
                  <p>Final 3D Print</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Refinement at CICS Makerspace</h4>
              <p>
                We transitioned to the CICS makerspace and found that the large plank was slightly too tall when screwed on top instead of the side. We sawed some off the top, then firmly mounted it to the base board. Finally, we drilled a hole in the top plank for the camera to view through.
              </p>
              
              <div className="hardware-image" style={{ maxWidth: '400px', margin: '1rem auto' }}>
                <img src="/HardwareDesign/Drilled_Hole.png" alt="Drilled Camera Hole" />
                <p>Drilled Camera Hole</p>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Final Mount Design</h4>
              <p>
                Once the hole was drilled, we needed a method to standardize the position of the ESP32 and ToF sensors. We iterated through four versions of the top mount:
              </p>
              <ul>
                <li>Started with just mounting holes for the ToF sensors</li>
                <li>Found an angle that prevented noise from hand detection while detecting gestures</li>
                <li>Settled on an 8-degree upward angle for each sensor with poles to screw into</li>
                <li>Added arms to guide the ESP32's weight, keeping it in a constant position for camera reference</li>
              </ul>
              <p>
                The mount tightly slid onto the wood base with spots for three ToF sensors. However, during final assembly, some of the ToF mounts broke in the middle, forcing us to transition to only two ToF sensors.
              </p>
              
              <div className="image-grid-four">
                <div className="hardware-image">
                  <img src="/HardwareDesign/TOF_Mounts/TOF_CAD.png" alt="TOF Mount CAD Design" />
                  <p>TOF Mount CAD Design</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/TOF_Mounts/TOF_Second_Iteration.png" alt="TOF Mount Second Iteration" />
                  <p>TOF Mount Second Iteration</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/TOF_Mounts/TOF_Full_Iteration.png" alt="TOF Mount Full Iteration" className="rotate-90-ccw" />
                  <p>TOF Mount Full Iteration</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/TOF_Mounts/TOF_Final.png" alt="Final TOF Mount" />
                  <p>Final TOF Mount</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Paint and Finishing</h4>
              <p>
                To help our inference model, we painted everything matte black, opting for the darkest background possible to eliminate noise from the camera feed.
              </p>
              
              <div className="hardware-image" style={{ maxWidth: '400px', margin: '1rem auto' }}>
                <img src="/HardwareDesign/Painted_Black.png" alt="Painted Black Apparatus" />
                <p>Painted Black Apparatus</p>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Final Assembly</h4>
              <p>
                Because the serial clock was broken on our ESP32-S3 Cam (which we had been using the entire time), we were forced to attach a MakerBoard from class to connect all the ToF sensors.
              </p>
              
              <div className="image-grid">
                <div className="hardware-image">
                  <img src="/HardwareDesign/Final_Assembly/From_Front.png" alt="Final Assembly Front View" />
                  <p>Final Assembly Front View</p>
                </div>
                <div className="hardware-image">
                  <img src="/HardwareDesign/Final_Assembly/In_Demo.png" alt="Final Assembly In Demo" className="zoomed-out" />
                  <p>Final Assembly In Demo</p>
                </div>
              </div>
            </div>

          </div>
        )}
        
        {selectedStep === 'handdetection' && (
          <div className="step-details">
            <div className="step-header">
              <h3>✋ Hand Detection Model</h3>
              <div className="team-attribution">Team: Austin</div>
            </div>
            
            <div className="hardware-intro">
              <p>
                Our hand detection system uses a CNN-based model running on an ESP32-S3 with an OV2640 camera. The journey from concept to final model involved multiple iterations, dataset refinements, and hardware constraints that shaped our approach.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Camera Setup & Data Collection</h4>
              <p>
                We started by setting up the OV2640 camera and building a simple web app to receive the stream. After testing functionality and pin configurations, we moved to data collection using a custom setup that captured images at the native 160x120 resolution.
              </p>
              <p>
                Initially mounted with black construction paper as a background, we later transitioned to black painted wood framing for better consistency. Using an autoclicker, we captured approximately 3 pictures per second, varying hand positions to account for different hover levels, rotations, and angles. We also added shading to simulate shadows during demos.
              </p>
              
              <div className="image-grid">
                <div className="hardware-image">
                  <img src="/HandDetector/Initial_View_From_Plank.png" alt="Initial Camera View" />
                  <p>Initial View From Mounted Camera</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Data_Collection.png" alt="Data Collection Interface" />
                  <p>Data Collection Web Interface</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Image Preprocessing Pipeline</h4>
              <p>
                Our preprocessing pipeline was designed to optimize for the ESP32's constraints while maintaining model accuracy:
              </p>
              <ul>
                <li>Crop images from 160x120 to 106x94</li>
                <li>Rescale to 48x48 (final resolution after testing)</li>
                <li>Convert to grayscale</li>
                <li>Color augmentation during training to improve inference on darker skin tones</li>
              </ul>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HandDetector/Open_After_Processing.png" alt="Open Hand After Processing" />
                  <p>Open Hand After Processing</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Closed_After_Processing.png" alt="Closed Hand After Processing" />
                  <p>Closed Hand After Processing</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Augmented_Dark.png" alt="Augmented Dark Image" />
                  <p>Color Augmentation for Dark Skin</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Phase 1: Initial Six Gestures</h4>
              <p>
                We started ambitious with six different hand gestures: thumbs up (like song), thumbs down (unlike song), open hand (stop), closed hand (play), point right (skip), and point left (rewind).
              </p>
              <p>
                <strong>The Problem:</strong> Images were too noisy with varied backgrounds. We realized we needed to focus the camera downward at a blank background. Additionally, the ESP32's size constraints meant we needed a much smaller model than this complex dataset required.
              </p>
              
              <div className="image-grid-four">
                <div className="hardware-image">
                  <img src="/HandDetector/Phase1/Thumbs_Up_68.jpg" alt="Phase 1 Thumbs Up" />
                  <p>Phase 1: Thumbs Up</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase1/Thumbs_Down_166.jpg" alt="Phase 1 Thumbs Down" />
                  <p>Phase 1: Thumbs Down</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase1/Open_Hand_966.jpg" alt="Phase 1 Open Hand" />
                  <p>Phase 1: Open Hand</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase1/Point_Left_741.jpg" alt="Phase 1 Point Left" />
                  <p>Phase 1: Point Left</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Phase 2: Simplified Gestures</h4>
              <p>
                After realizing the hand signals weren't different enough (some looked very similar after preprocessing), we simplified to three gestures with approximately 500 images each: peace sign (like song), open hand (play), and closed hand (pause).
              </p>
              <p>
                <strong>The Problem:</strong> The peace sign was still being inferred as an open hand. Two fingers weren't distinguishable enough for our small CNN, even with the 64x64 model.
              </p>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HandDetector/Phase2/Peace_Sign_1233.jpg" alt="Phase 2 Peace Sign" />
                  <p>Phase 2: Peace Sign</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase2/Open_Hand_198.jpg" alt="Phase 2 Open Hand" />
                  <p>Phase 2: Open Hand</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase2/Closed_Hand_632.jpg" alt="Phase 2 Closed Hand" />
                  <p>Phase 2: Closed Hand</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Phase 3: Adding Noise Detection</h4>
              <p>
                Data was collected in the CS Makerspace (similar to our production environment) with approximately 500 images per class. We kept open hand (play) and closed hand (pause), but added a crucial third class: "None" to classify accidental triggers and noise.
              </p>
              <p>
                <strong>The Challenge:</strong> While test set accuracy was 100%, the model struggled with real-world noise and may have been overfit. More critically, inference took approximately 5 seconds with the 64x64 input scale, which was far too slow for real-time use.
              </p>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HandDetector/Phase3/Open_Hand_271.jpg" alt="Phase 3 Open Hand" />
                  <p>Phase 3: Open Hand</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase3/Closed_Hand_569.jpg" alt="Phase 3 Closed Hand" />
                  <p>Phase 3: Closed Hand</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Phase3/None_256.jpg" alt="Phase 3 None" />
                  <p>Phase 3: None (Background)</p>
                </div>
              </div>
            </div>

            <div className="hardware-section">
              <h4>Model Speed Testing & Optimization</h4>
              <p>
                We profiled our inference code into discrete processes: camera capture, image preprocessing, filling input tensor, running model inference, processing output, and results summary. This allowed us to compare models with different input and hidden layer sizes.
              </p>
              <p>
                <strong>Key Findings:</strong>
              </p>
              <ul>
                <li>64x64 model: ~5 seconds inference time (too slow)</li>
                <li>32x32 model: ~0.5 seconds inference time (not accurate enough)</li>
                <li>48x48 model: ~1.5 seconds inference time (sweet spot!)</li>
              </ul>
              <p>
                The 48x48 resolution provided a good balance between inference time and predictive performance.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Final Model: Production Ready</h4>
              <p>
                We collected final data in LGRC A104A to capture real production lighting conditions. With approximately 1,000 images per class, we significantly increased training data to better capture variance. Different hand angles, rotations, and lighting conditions (using a computer case to create shadows) were all included.
              </p>
              
              <div className="hardware-image" style={{ maxWidth: '400px', margin: '1rem auto' }}>
                <img src="/HandDetector/Shadows.png" alt="Shadow Variations" />
                <p>Shadow Variation Testing</p>
              </div>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HandDetector/Final/Open_Hand_223.jpg" alt="Final Open Hand 1" />
                  <p>Final: Open Hand Example 1</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Final/Open_Hand_361.jpg" alt="Final Open Hand 2" />
                  <p>Final: Open Hand Example 2</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Final/Closed_Hand_658.jpg" alt="Final Closed Hand 1" />
                  <p>Final: Closed Hand Example 1</p>
                </div>
              </div>
              
              <div className="image-grid-three">
                <div className="hardware-image">
                  <img src="/HandDetector/Final/Closed_Hand_1472.jpg" alt="Final Closed Hand 2" />
                  <p>Final: Closed Hand Example 2</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Final/None_2381.jpg" alt="Final None 1" />
                  <p>Final: None Example 1</p>
                </div>
                <div className="hardware-image">
                  <img src="/HandDetector/Final/None_2565.jpg" alt="Final None 2" />
                  <p>Final: None Example 2</p>
                </div>
              </div>
              
              <p>
                <strong>Model Optimizations:</strong>
              </p>
              <ul>
                <li>Quantized model from fp32 to int8 (4x size reduction)</li>
                <li>Changed input size and hidden layers from 64x64 to 48x48 scale</li>
                <li>Final inference time: ~1.5 seconds</li>
              </ul>
              
              <div className="hardware-image" style={{ maxWidth: '500px', margin: '1rem auto' }}>
                <img src="/HandDetector/Learning_Curve.png" alt="Learning Curve" />
                <p>Model Training Learning Curve</p>
              </div>
              
              <p>
                The final model performed exceptionally well with 80%+ accuracy on images with 5% gaussian noise and perfect accuracy on rotated images. The increased training data and augmentation for darker skin tones made it robust across different conditions.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Capture Triggering System</h4>
              <p>
                Initially, we planned to use a distance sensor facing down to trigger captures. However, the 3D printed mount broke and we ran out of serial communication ports.
              </p>
              <p>
                <strong>Solution: Mean Filter Triggering</strong>
              </p>
              <p>
                We implemented a mean grayscale pixel value filter for the entire image. When the mean value rises above a threshold (indicating a hand is present), a capture signal is given.
              </p>
              <p>
                <strong>Signal Debouncing:</strong> After the initial trigger, the system waits 200ms and checks again to ensure the mean filter is still active. This accounts for swiping noise from gestures intended for the distance sensor and prevents false triggers.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Spotify Communication</h4>
              <p>
                The final inference result is simplified and printed to serial. A Python serial monitor on the connected computer checks for specific command phrases and communicates with the Spotify API to control playback. This keeps the ESP32 focused solely on inference while offloading API communication to the computer.
              </p>
            </div>

          </div>
        )}
        
        {selectedStep === 'gesturedetection' && (
          <div className="step-details">
            <div className="step-header">
              <h3>👋 Gesture Detection</h3>
              <div className="team-attribution">Team: Sam Newman</div>
            </div>
            
            <div className="hardware-intro">
              <p>
                The gesture detection system uses Time-of-Flight (ToF) distance sensors to enable intuitive hand gestures for controlling music playback. Two sensors angled slightly outward detect swipes for skipping/rewinding songs and proximity for volume control.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Sensor Setup & Configuration</h4>
              <p>
                Setting up the ToF sensors required careful initialization. Since multiple sensors share the same I2C address by default, we used the XSET pins to shut down each sensor individually and boot them one at a time, assigning unique serial addresses. This allows the system to distinguish between sensors and determine swipe direction.
              </p>
              <p>
                The sensors were angled slightly outward to ensure one would activate first, creating a distinct sequence even with fast swipes. We used a baseline detection distance of 30cm, though this is adjustable based on your specific setup and the distance to the nearest resting object.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Swipe Detection Logic</h4>
              <p>
                The swipe detection algorithm tracks which sensor was triggered last. When the opposite sensor registers a reading within 2 seconds, a swipe is confirmed in the corresponding direction:
              </p>
              <ul>
                <li><strong>Left to Right:</strong> Skip to next song</li>
                <li><strong>Right to Left:</strong> Go to previous song</li>
              </ul>
              <p>
                This timing window ensures intentional swipes are captured while filtering out accidental triggers. All timing parameters are tweakable and were tuned through extensive testing to work best with natural hand movements.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Volume Control Mode</h4>
              <p>
                A clever mode-switching mechanism allows the same sensors to control volume. The system continuously stores readings from the first sensor and calculates the average over a set time period. When this average drops below a threshold (indicating sustained hand presence), the system switches to volume mode.
              </p>
              <p>
                In volume mode, the sensor continuously reads hand distance and maps it to volume level. Closer hand position increases volume, farther decreases it. The Spotify API on the host computer receives these volume adjustments in real-time, creating a theremin-like control experience.
              </p>
              <p>
                Once the hand is removed and the sensor readings return to baseline, the system automatically exits volume mode and returns to swipe detection.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Code Implementation</h4>
              <p>
                The Arduino ESP32 C++ code handles all sensor logic and mode switching. Key components include:
              </p>
              <ul>
                <li>Sensor initialization with unique I2C addresses via XSET pins</li>
                <li>30cm baseline detection threshold</li>
                <li>2-second window for swipe confirmation</li>
                <li>Rolling average calculation for volume mode triggering</li>
                <li>Distance-to-volume mapping function</li>
              </ul>
              <p>
                The code is designed to be modular and adjustable. All threshold values, timing windows, and detection ranges can be tuned based on your specific hardware setup and user preferences. The full implementation can be found in <code>WaveDetector/src/DistanceSensorTheremin.ino</code>.
              </p>
            </div>

            <div className="demo-section">
              <h4>See It In Action</h4>
              <p>
                Check out the demo video in the <strong>Demo</strong> section to see the gesture detection system working in real-time, including swipe detection and volume control mode.
              </p>
            </div>

          </div>
        )}
        
        {selectedStep === 'integration' && (
          <div className="step-details">
            <div className="step-header">
              <h3>🔗 System Integration</h3>
              <div className="team-attribution">Team: Sam Newman & Ian Rapko</div>
            </div>
            
            <div className="hardware-intro">
              <p>
                Integrating all components into a working system required connecting the ESP32 boards, serial communication, a Python relay script, an Express.js web server, and the Spotify API. The architecture evolved to handle hardware constraints while maintaining reliable real-time control.
              </p>
            </div>

            <div className="hardware-section">
              <h4>System Architecture</h4>
              <p>
                The final system uses a multi-layered architecture:
              </p>
              <ul>
                <li><strong>ESP32 Boards:</strong> Two separate boards handle hand detection (ML model) and gesture detection (ToF sensors)</li>
                <li><strong>Serial Communication:</strong> Both boards output commands via USB serial to the host computer</li>
                <li><strong>Python Serial Receiver:</strong> Monitors serial ports and relays commands to the web server</li>
                <li><strong>Express.js Web Server:</strong> Hosts endpoints and communicates with Spotify API</li>
                <li><strong>Spotify API:</strong> Controls playback on the host computer</li>
              </ul>
            </div>

            <div className="hardware-section">
              <h4>Serial Communication Pipeline</h4>
              <p>
                The Python serial receiver serves as the bridge between hardware and software. Running in a continuous loop, it reads serial output from both ESP32 boards and analyzes the text for specific keywords:
              </p>
              <ul>
                <li><code>NEXT</code> - Skip to next song</li>
                <li><code>PREV</code> - Go to previous song</li>
                <li><code>VOL</code> - Adjust volume (with level parameter)</li>
                <li><code>PLAY</code> - Resume playback</li>
                <li><code>PAUSE</code> - Pause playback</li>
              </ul>
              <p>
                When a keyword is detected, the receiver immediately sends a POST request to the corresponding Express server endpoint. While this could have been integrated directly into the Express server, reading serial input in Python proved simpler and more reliable, despite creating a slightly more complex data flow.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Express.js Web Server</h4>
              <p>
                The web server, built with JavaScript and Express.js, hosts POSTable endpoints that the serial receiver interacts with. Each endpoint corresponds to a specific Spotify action:
              </p>
              <ul>
                <li><code>POST /next</code> - Skip track</li>
                <li><code>POST /previous</code> - Previous track</li>
                <li><code>POST /volume</code> - Set volume level</li>
                <li><code>POST /play</code> - Resume playback</li>
                <li><code>POST /pause</code> - Pause playback</li>
              </ul>
              <p>
                When an endpoint receives a request, the server forwards the command to the appropriate Spotify Web API endpoint along with authentication credentials. This separation of concerns keeps the embedded code simple while handling all API complexity on the host computer.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Spotify API Authentication Challenge</h4>
              <p>
                The Spotify API authentication proved to be one of the most challenging aspects of the project. Initially, we attempted to use simple Client ID and Secret authentication, which only provides read access to public information like artist names and song titles.
              </p>
              <p>
                <strong>The Problem:</strong> Controlling playback on a host computer requires OAuth 2.0 authentication with user permission.
              </p>
              <p>
                <strong>The Solution:</strong> We implemented a full OAuth flow that requires users to visit the <code>/login</code> page. This redirects to Spotify's third-party login, which provides an authorization code. The code is then exchanged for access and refresh tokens, which are stored server-side for subsequent API calls.
              </p>
              <p>
                While more complex than anticipated, this authentication method provides full control over the user's active Spotify session. Future improvements could store tokens in environment variables for longer-term persistence.
              </p>
            </div>

            <div className="hardware-section">
              <h4>The Two-Board Solution</h4>
              <p>
                The night before the project deadline, we encountered a critical hardware failure: the serial communication ports on our ESP32-S3 (running the ML model) stopped working. This forced a last-minute pivot to the current two-board architecture.
              </p>
              <p>
                <strong>Current Setup:</strong>
              </p>
              <ul>
                <li><strong>Board 1:</strong> ESP32-S3 with OV2640 camera running hand detection ML model</li>
                <li><strong>Board 2:</strong> Separate ESP32 with ToF sensors for gesture detection</li>
                <li><strong>MakerBoard:</strong> Additional board for ToF sensor connections (since S3 I2C was compromised)</li>
                <li><strong>Two Serial Monitors:</strong> One Python script per board, both posting to the same web server</li>
              </ul>
              <p>
                While not the original plan, this separation has benefits. The two code bases remain independent and simpler, as the ML inference code doesn't need to be integrated with the sensor logic. Each board can be developed, tested, and debugged independently.
              </p>
            </div>

            <div className="hardware-section">
              <h4>Integration Challenges & Solutions</h4>
              <p>
                Throughout the integration process, we encountered several challenges:
              </p>
              <ul>
                <li><strong>Serial Port Conflicts:</strong> Running multiple serial connections required careful port management and separate Python instances</li>
                <li><strong>Timing Synchronization:</strong> Ensuring commands from both boards didn't conflict required debouncing and priority logic</li>
                <li><strong>API Rate Limiting:</strong> Spotify API has rate limits; we implemented request throttling to prevent errors</li>
                <li><strong>Token Refresh:</strong> Access tokens expire; automatic refresh logic prevents authentication failures</li>
                <li><strong>Error Handling:</strong> Robust error handling ensures the system continues working even if one component fails</li>
              </ul>
            </div>

            <div className="hardware-section">
              <h4>Data Flow Summary</h4>
              <p>
                The complete data flow from gesture to Spotify action:
              </p>
              <ol>
                <li>User performs hand gesture or swipe</li>
                <li>ESP32 board detects gesture and outputs command via serial</li>
                <li>Python serial receiver reads serial output and identifies keyword</li>
                <li>Receiver sends POST request to Express server endpoint</li>
                <li>Express server authenticates and forwards request to Spotify API</li>
                <li>Spotify API controls playback on host computer</li>
                <li>User hears the result (song skip, volume change, etc.)</li>
              </ol>
              <p>
                This multi-hop architecture allows for real-time control with latency typically under 500ms from gesture to action.
              </p>
            </div>

            <div className="demo-section">
              <h4>See The Full System</h4>
              <p>
                Watch the complete system in action in the <strong>Demo</strong> section, where you can see hand detection, gesture control, and Spotify integration working together seamlessly.
              </p>
            </div>

          </div>
        )}
        
      </div>
    </div>
  )
}

function App() {
  const [activeTab, setActiveTab] = useState('model')

  return (
    <div className="App">
      <header className="App-header">
        <h1>SpotifyGesture</h1>
        <p>Gesture-Controlled Music Experience</p>
      </header>
      
      <nav className="tabs">
        <button 
          className={`tab ${activeTab === 'model' ? 'active' : ''}`}
          onClick={() => setActiveTab('model')}
        >
          3D Model
        </button>
        <button 
          className={`tab ${activeTab === 'overview' ? 'active' : ''}`}
          onClick={() => setActiveTab('overview')}
        >
          Project Overview
        </button>
        <button 
          className={`tab ${activeTab === 'engineering' ? 'active' : ''}`}
          onClick={() => setActiveTab('engineering')}
        >
          Engineering Process
        </button>
        <button 
          className={`tab ${activeTab === 'team' ? 'active' : ''}`}
          onClick={() => setActiveTab('team')}
        >
          Team
        </button>
        <button 
          className={`tab ${activeTab === 'demo' ? 'active' : ''}`}
          onClick={() => setActiveTab('demo')}
        >
          Demo
        </button>
        <button 
          className={`tab ${activeTab === 'source' ? 'active' : ''}`}
          onClick={() => setActiveTab('source')}
        >
          Source Code
        </button>
      </nav>
      
      <main className="main-content">
        <section className={`viewer-section ${activeTab === 'model' ? 'expanded' : 'collapsed'}`}>
          <div className="section-header">
            <h2>3D Model</h2>
            <p className="controls-hint">Rotate: Left Click + Drag | Zoom: Scroll | Pan: Right Click + Drag</p>
          </div>
          <div className="viewer-container">
            <ModelViewer />
          </div>
        </section>
        
        <section className={`content-section ${activeTab !== 'model' ? 'expanded' : 'collapsed'}`}>
          <div className="content-wrapper">
            {activeTab === 'overview' && (
              <div className="tab-content">
                <h2>Project Overview</h2>
                
                <div className="overview-intro">
                  <p>
                    SpotifyGesture is a hands-free music control system that lets you control Spotify using intuitive hand gestures and swipes. Built as our final project, it combines computer vision, embedded systems, and web technologies to create a seamless music experience without ever touching your device.
                  </p>
                </div>

                <div className="overview-section">
                  <h3>How It Works</h3>
                  <p>
                    The system features two main control methods:
                  </p>
                  <ul>
                    <li><strong>Gesture Control (ToF Sensors):</strong> Swipe right to skip songs, swipe left to go back. Hold your hand over the right sensor for 2 seconds to enter volume mode, then control volume by moving your hand closer or farther away.</li>
                    <li><strong>Hand Signal Control (Camera + ML):</strong> Position your hand under the camera with the back of your hand facing up. Make an open hand to play, or a closed fist to pause.</li>
                  </ul>
                  
                  <div className="overview-image">
                    <img src="/Demo_Day_Slide.png" alt="Demo Day Presentation Slide" />
                    <p>Our Demo Day presentation slide</p>
                  </div>
                  
                  <div className="overview-image" style={{ marginTop: '2rem' }}>
                    <img src="/Demo_Day.png" alt="Demo Day Setup" />
                    <p>Our project setup at Demo Day</p>
                  </div>
                </div>

                <div className="overview-section">
                  <h3>Final Hardware Components</h3>
                  <div className="hardware-list">
                    <div className="hardware-item">
                      <span className="hw-quantity">2x</span>
                      <span className="hw-name">VL53L0X Time-of-Flight Distance Sensors</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">1x</span>
                      <span className="hw-name">OV2640 Camera Module</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">1x</span>
                      <span className="hw-name">ESP32-S3 CAM (16MB Flash, 8MB PSRAM)</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">1x</span>
                      <span className="hw-name">MakerBoard (for sensor connections)</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">15+</span>
                      <span className="hw-name">Jumper Wires</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">1x</span>
                      <span className="hw-name">Speaker (audio feedback)</span>
                    </div>
                    <div className="hardware-item">
                      <span className="hw-quantity">2x</span>
                      <span className="hw-name">USB-C Cables</span>
                    </div>
                  </div>
                </div>

                <div className="overview-section success-section">
                  <h3>Demo Day Success</h3>
                  <p>
                    Our project was a success at Demo Day! We received an <strong>A grade</strong> and were recognized as being in the <strong>top 40% of projects</strong> in the class. The system worked reliably for most participants who tried it out, and we even set up a Spotify Jam so people could play their own songs while testing the gesture controls.
                  </p>
                  <p>
                    One of the highlights was walking around and checking out all the other amazing projects from our classmates. It was inspiring to see the creativity and technical skill on display, and we loved sharing our work with everyone who stopped by our station.
                  </p>
                </div>

                <div className="overview-section">
                  <h3>Key Challenges & Solutions</h3>
                  
                  <div className="challenge-card">
                    <h4>Running ML on Embedded Hardware</h4>
                    <p>
                      <strong>The Problem:</strong> Our initial hand detection model was far too large for the ESP32-S3, causing inference times of 3-4 seconds. This latency made gesture recognition completely impractical for real-time use.
                    </p>
                    <p>
                      <strong>Our Solution:</strong> We took an iterative approach to optimization. We systematically reduced model complexity by shrinking layer sizes, reducing parameters, and experimenting with different quantization techniques. By decreasing the input image size from 64x64 to 48x48 and quantizing from fp32 to int8, we brought inference time down to approximately 1.5 seconds while maintaining accuracy. We quantitatively measured time-to-inference at each step, ensuring we maximized model capacity while achieving seamless integration.
                    </p>
                  </div>

                  <div className="challenge-card">
                    <h4>Spotify API Integration</h4>
                    <p>
                      <strong>The Problem:</strong> Integrating the Spotify Web API proved more difficult than anticipated. We struggled with OAuth authentication flow, maintaining active sessions, and implementing everything within the constrained ESP32 environment.
                    </p>
                    <p>
                      <strong>Our Solution:</strong> Rather than trying to get everything working simultaneously, we broke the problem down. We first implemented and tested local audio playback features directly on the board, giving us a working baseline system. With that foundation in place, we could then layer in the Spotify API integration and gesture controls one piece at a time. We also moved API handling to a Python script and Express server on the host computer, keeping the embedded code focused solely on detection and serial communication.
                    </p>
                  </div>

                  <div className="challenge-card">
                    <h4>System Integration</h4>
                    <p>
                      <strong>The Problem:</strong> With limited time, bringing all the separate components together into a single working product was challenging. The distance sensors, camera, ML model, serial communication, and Spotify API each worked independently, but combining them revealed timing conflicts and communication issues.
                    </p>
                    <p>
                      <strong>Our Solution:</strong> When the serial ports on our ESP32-S3 failed the night before the deadline, we pivoted to a two-board architecture. While not the original plan, this separation actually simplified debugging and allowed each subsystem to remain independent. We also implemented careful debouncing and priority logic to prevent command conflicts between the two input methods.
                    </p>
                  </div>
                </div>

                <div className="overview-section">
                  <h3>Lessons Learned</h3>
                  <ul>
                    <li><strong>Hardware constraints drive design:</strong> For local inference, we had to adapt our model size to match the hardware capabilities. What seems standard on a desktop becomes impossible on embedded hardware.</li>
                    <li><strong>Iterative optimization works:</strong> By measuring performance at each step and making incremental improvements, we found the sweet spot between accuracy and speed.</li>
                    <li><strong>Pivot when necessary:</strong> We completely changed our project scope due to unforeseen circumstances with our original proposal (Olympia Place), moving to this similarly complex but more feasible project.</li>
                    <li><strong>Plan for space constraints:</strong> Integrating a screen with live music display on such a small board proved difficult. If we had more time, we should have ordered a custom PCB with space constraints in mind from the start.</li>
                    <li><strong>Sometimes failures lead to better solutions:</strong> The two-board architecture that resulted from our hardware failure actually made the system easier to develop and debug.</li>
                  </ul>
                </div>

                <div className="overview-section team-section">
                  <h3>The Team</h3>
                  <div className="team-grid">
                    <div className="team-member">
                      <h4>Austin</h4>
                      <p>Hand detection ML model, camera integration, apparatus design</p>
                    </div>
                    <div className="team-member">
                      <h4>Ian</h4>
                      <p>Hardware integration, screen testing, code migration, apparatus construction</p>
                    </div>
                    <div className="team-member">
                      <h4>Sam</h4>
                      <p>Gesture detection, ToF sensors, Spotify API integration, serial communication</p>
                    </div>
                  </div>
                </div>

                <div className="future-work-section">
                  <h3>Future Work</h3>
                  <p>
                    While we're proud of what we accomplished, there are several improvements we'd love to implement given more time:
                  </p>
                  <ul>
                    <li><strong>Single Board Integration:</strong> Consolidate all functionality onto one ESP32 board to reduce complexity and improve reliability</li>
                    <li><strong>Compact Design:</strong> Redesign the apparatus with a smaller footprint and more portable form factor</li>
                    <li><strong>OLED Screen Integration:</strong> Successfully implement the display to show real-time song information and gesture feedback</li>
                    <li><strong>Enhanced Spotify API Integration:</strong> Add visual feedback for playback state, volume levels, and currently playing tracks</li>
                    <li><strong>Audio Feedback:</strong> Integrate a buzzer or speaker to provide immediate audio confirmation when inference completes</li>
                    <li><strong>Custom PCB Design:</strong> Create a purpose-built circuit board with proper space allocation for all components</li>
                    <li><strong>Additional Gestures:</strong> Expand the gesture vocabulary to include more controls like playlist navigation and repeat/shuffle modes</li>
                  </ul>
                </div>

              </div>
            )}
            
            {activeTab === 'engineering' && (
              <div className="tab-content">
                <h2>Engineering Process</h2>
                <EngineeringProcess />
              </div>
            )}
            
            {activeTab === 'team' && (
              <div className="tab-content">
                <h2>The Team</h2>
                
                <div className="team-intro">
                  <img src="/All_People.jpg" alt="Team Photo" className="team-photo" />
                  <p>
                    SpotifyGesture was built by three UMass Amherst computer science students who combined their expertise in machine learning, systems integration, and embedded development to create an innovative hands-free music control system.
                  </p>
                </div>

                <div className="team-members-detailed">
                  
                  <div className="member-card-detailed">
                    <div className="member-header">
                      <img src="/Austin.png" alt="Austin Fairbanks" className="member-photo-detailed" />
                      <div className="member-title-section">
                        <h3>Austin Fairbanks</h3>
                        <p className="member-role">Machine Learning & Model Development</p>
                        <div className="member-links">
                          <a href="https://ajfairbanks.me/" target="_blank" rel="noopener noreferrer">🌐 Website</a>
                          <a href="https://www.linkedin.com/in/ajf2005/" target="_blank" rel="noopener noreferrer">💼 LinkedIn</a>
                        </div>
                      </div>
                    </div>
                    <div className="member-details">
                      <p className="member-bio">
                        Incoming Data Scientist @ Zipline | Former ML Engineer @ Corning Incorporated | B.S. Computer Science 2026, M.S. 2027 at UMass Amherst
                      </p>
                      <p>
                        Austin led the development of the hand detection model, tackling the challenge of running a CNN on the ESP32-S3's limited hardware. Through iterative optimization, he reduced inference time from 5 seconds to 1.5 seconds while maintaining accuracy. His experience with GPU-accelerated computing from his Corning internship proved invaluable in benchmarking different approaches.
                      </p>
                      <p className="member-focus">
                        <strong>Focus Areas:</strong> Edge machine learning, embedded inference optimization, computer vision, physical computing
                      </p>
                      <p className="member-contribution">
                        <strong>Key Contributions:</strong> CNN model architecture, quantization (fp32 → int8), data collection pipeline, camera integration, apparatus design, model training & optimization
                      </p>
                    </div>
                  </div>

                  <div className="member-card-detailed">
                    <div className="member-header">
                      <img src="/Ian.png" alt="Ian Rapko" className="member-photo-detailed" />
                      <div className="member-title-section">
                        <h3>Ian Rapko</h3>
                        <p className="member-role">Hardware Integration & System Development</p>
                        <div className="member-links">
                          <a href="https://www.iann.dev/work.html" target="_blank" rel="noopener noreferrer">🌐 Website</a>
                          <a href="https://www.linkedin.com/in/ian-rapko/" target="_blank" rel="noopener noreferrer">💼 LinkedIn</a>
                        </div>
                      </div>
                    </div>
                    <div className="member-details">
                      <p className="member-bio">
                        Tech Intern @ Johnson & Johnson | Computer Science Student at UMass Amherst | Founder of SecureDAO ($3M valuation) & SilentBball (5M+ impressions)
                      </p>
                      <p>
                        Ian focused on hardware integration and system development, working extensively on OLED screen implementation for live music display and feedback. He also played a crucial role in code migration between boards and apparatus construction. While the screen integration proved challenging due to space constraints on the ESP32, Ian's efforts in testing different display configurations and optimizing code for the limited hardware were invaluable to the development process.
                      </p>
                      <p className="member-focus">
                        <strong>Focus Areas:</strong> Hardware integration, embedded displays, code migration, physical apparatus construction
                      </p>
                      <p className="member-contribution">
                        <strong>Key Contributions:</strong> OLED screen testing & implementation, code migration between boards, apparatus construction at makerspaces, hardware assembly, system integration support
                      </p>
                    </div>
                  </div>

                  <div className="member-card-detailed">
                    <div className="member-header">
                      <img src="/Sam.png" alt="Sam Newman" className="member-photo-detailed" />
                      <div className="member-title-section">
                        <h3>Sam Newman</h3>
                        <p className="member-role">Embedded Systems, API Integration & Gesture Detection</p>
                        <div className="member-links">
                          <a href="https://newmie10.github.io/" target="_blank" rel="noopener noreferrer">🌐 Website</a>
                          <a href="https://www.linkedin.com/in/sam-newman-umass/" target="_blank" rel="noopener noreferrer">💼 LinkedIn</a>
                        </div>
                      </div>
                    </div>
                    <div className="member-details">
                      <p className="member-bio">
                        Computer Science Student at UMass Amherst | Commonwealth Honors College | 4.0 GPA | Cybersecurity & Low-Level Development Enthusiast
                      </p>
                      <p>
                        Sam handled both the embedded sensor systems and the complete Spotify API integration. He developed the ToF sensor-based gesture detection system with swipe recognition and volume control algorithms, while also building the entire backend infrastructure for Spotify communication. His focus on cybersecurity and low-level development, including CTF competitions, gave him the expertise to handle everything from I2C sensor communication to OAuth authentication flows.
                      </p>
                      <p className="member-focus">
                        <strong>Focus Areas:</strong> Embedded systems, API integration, cybersecurity, low-level development, full-stack connectivity
                      </p>
                      <p className="member-contribution">
                        <strong>Key Contributions:</strong> ToF sensor setup, swipe detection algorithm, volume control mode, Spotify API integration, OAuth implementation, Express.js server, Python serial receiver, Arduino C++ implementation, system architecture
                      </p>
                    </div>
                  </div>

                </div>

                <div className="collaboration-section">
                  <h3>Team Collaboration</h3>
                  <p>
                    While each team member had their primary focus areas, this project was truly collaborative. All three members contributed to the apparatus design and hardware assembly at the CICS and all-campus makerspaces. Ian worked extensively on screen implementation and code migration, though we ultimately ran out of time to integrate the OLED display due to space constraints on the ESP32. We held regular integration sessions to ensure the separate subsystems worked together seamlessly, and troubleshot hardware issues as a team—especially during the critical serial port failure that led to our two-board architecture.
                  </p>
                  <p>
                    The project brought together diverse skill sets: Austin's ML optimization expertise, Ian's hardware integration and system development work, and Sam's full-stack embedded-to-cloud connectivity spanning from I2C sensors to Spotify APIs. This combination allowed us to tackle challenges that spanned from model quantization to screen implementation to real-time sensor processing and OAuth flows.
                  </p>
                </div>

                <div className="acknowledgments-section">
                  <h3>Acknowledgments</h3>
                  <p>
                    We would like to thank <strong>Professor Md Farhan Tanism</strong> and the CICS 256 course staff for their guidance and support throughout this project. Special thanks to the staff at both the <strong>UMass CICS Makerspace</strong> and the <strong>All-Campus Makerspace</strong> for providing the facilities, tools, and expertise that made our apparatus construction possible. Their assistance with woodworking, drilling, 3D printing, and general hardware troubleshooting was invaluable to bringing this project to life.
                  </p>
                </div>

              </div>
            )}
            
            {activeTab === 'demo' && (
              <div className="tab-content">
                <h2>Demo Videos</h2>
                
                <div className="demo-intro">
                  <p>
                    Watch SpotifyGesture in action! These videos demonstrate the complete system working with both gesture detection and hand signal recognition.
                  </p>
                </div>

                <div className="video-section">
                  <h3>POV Demo</h3>
                  <p>
                    First-person view showing the user's perspective while controlling Spotify with hand gestures and signals. See how the system responds to swipes, volume control, and hand positions in real-time.
                  </p>
                  <div className="video-container">
                    <video controls>
                      <source src="/POV_Demo.mp4" type="video/mp4" />
                      Your browser does not support the video tag.
                    </video>
                  </div>
                </div>

                <div className="video-section">
                  <h3>Third Person Demo</h3>
                  <p>
                    External view showing the complete setup and how users interact with the system. This angle shows the apparatus, hand movements, and the overall user experience.
                  </p>
                  <div className="video-container">
                    <video controls>
                      <source src="/Third_Person_Demo.mp4" type="video/mp4" />
                      Your browser does not support the video tag.
                    </video>
                  </div>
                </div>

                <div className="demo-features">
                  <h3>What You'll See</h3>
                  <ul>
                    <li>👋 <strong>Swipe Gestures:</strong> Left and right swipes to skip/rewind songs using ToF sensors</li>
                    <li>🔊 <strong>Volume Control:</strong> Distance-based volume adjustment by holding hand over sensor</li>
                    <li>✋ <strong>Hand Signals:</strong> Open palm to play, closed fist to pause using camera detection</li>
                    <li>⚡ <strong>Real-time Response:</strong> Low latency between gesture and Spotify action</li>
                    <li>🎵 <strong>Live Playback:</strong> Actual music control with visual feedback</li>
                  </ul>
                </div>

              </div>
            )}
            
            {activeTab === 'source' && (
              <div className="tab-content">
                <h2>Source Code</h2>
                
                <div className="github-main">
                  <div className="github-hero">
                    <div className="github-icon-large">⭐</div>
                    <h3>SpotifyGesture on GitHub</h3>
                    <p>
                      The complete source code for this project is available on GitHub. Explore the implementation details, model training code, embedded firmware, and Spotify API integration.
                    </p>
                    <a href="https://github.com/newmie10/SpotifyGesture" target="_blank" rel="noopener noreferrer" className="github-button-large">
                      View Repository on GitHub
                    </a>
                  </div>

                  <div className="repo-structure">
                    <h3>Repository Structure</h3>
                    <div className="folder-list">
                      <div className="folder-item">
                        <span className="folder-icon">📁</span>
                        <div className="folder-details">
                          <h4>HandDetector/</h4>
                          <p>Hand detection ML model training code, data preprocessing, and model quantization for ESP32 deployment. Includes training notebooks, model architecture, and inference testing.</p>
                        </div>
                      </div>
                      <div className="folder-item">
                        <span className="folder-icon">📁</span>
                        <div className="folder-details">
                          <h4>WaveDetector/</h4>
                          <p>Gesture detection using ToF sensors, Spotify API integration, and serial communication code. Contains the main embedded firmware for the ESP32 and sensor processing algorithms.</p>
                        </div>
                      </div>
                      <div className="folder-item">
                        <span className="folder-icon">📁</span>
                        <div className="folder-details">
                          <h4>project_display/</h4>
                          <p>This website! Built with Vite + React and React Three Fiber for 3D model visualization. Includes all the documentation and project showcase code.</p>
                        </div>
                      </div>
                    </div>
                  </div>

                  <div className="tech-stack">
                    <h3>Technologies Used</h3>
                    <div className="tech-grid">
                      <div className="tech-category">
                        <h4>Machine Learning</h4>
                        <ul>
                          <li>TensorFlow / TensorFlow Lite</li>
                          <li>Python (NumPy, Pandas)</li>
                          <li>Model Quantization (int8)</li>
                          <li>Image Preprocessing</li>
                        </ul>
                      </div>
                      <div className="tech-category">
                        <h4>Embedded Systems</h4>
                        <ul>
                          <li>ESP32-S3-CAM</li>
                          <li>Arduino / C++</li>
                          <li>I2C Communication</li>
                          <li>Serial Communication</li>
                        </ul>
                      </div>
                      <div className="tech-category">
                        <h4>Hardware</h4>
                        <ul>
                          <li>VL53L1X ToF Sensors (2x)</li>
                          <li>OV2640 Camera Module</li>
                          <li>Custom 3D Printed Mounts</li>
                          <li>Makerboard / ESP32</li>
                        </ul>
                      </div>
                      <div className="tech-category">
                        <h4>Web & APIs</h4>
                        <ul>
                          <li>Spotify Web API</li>
                          <li>OAuth 2.0 Authentication</li>
                          <li>React + Vite</li>
                          <li>Three.js / React Three Fiber</li>
                        </ul>
                      </div>
                    </div>
                  </div>

                  <div className="contributing">
                    <h3>Documentation</h3>
                    <p>
                      Each subdirectory contains detailed documentation about its specific component. Check out the README files and engineering process markdown documents in the repository for more technical details, setup instructions, and development notes.
                    </p>
                  </div>
                </div>

              </div>
            )}
          </div>
        </section>
      </main>
      
      <footer className="site-footer">
        <div className="footer-content">
          <div className="footer-section">
            <h4>SpotifyGesture</h4>
            <p>Gesture-Controlled Music Experience</p>
          </div>
          <div className="footer-section">
            <h4>Course</h4>
            <p>CICS 256: Make: A Hands-on Introduction to Physical Computing</p>
            <p>University of Massachusetts Amherst</p>
            <p>Fall 2025</p>
          </div>
          <div className="footer-section">
            <h4>Team</h4>
            <p>Austin Fairbanks</p>
            <p>Ian Rapko</p>
            <p>Sam Newman</p>
          </div>
          <div className="footer-section">
            <h4>Links</h4>
            <p><a href="https://github.com/newmie10/SpotifyGesture" target="_blank" rel="noopener noreferrer">GitHub Repository</a></p>
            <p><a href="https://www.cics.umass.edu/" target="_blank" rel="noopener noreferrer">UMass CICS</a></p>
          </div>
        </div>
        <div className="footer-bottom">
          <p>© 2025 SpotifyGesture Team. Built for CICS 256 at UMass Amherst.</p>
        </div>
      </footer>
    </div>
  )
}

export default App
