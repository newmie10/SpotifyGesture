Ok so far hardware design this was the following process

In our prelimiary investigation, (more info in the hand detection model tab), we discovered that images with a noisy background where going to be extremely difficult to denoise and do inferenc on. because of this, we knew we would have to point the camera and sensors down so we had an easier 'normal' state for sensors and camera readings.

Our first idea was to go to the all-campus makerspace nad construct a test wood platform to mount the camera and sensors on.  (insert picture of apparatus construction) we took a drill a few scrap pieces and screwed 2 poles, one around 16 inches in height and the other around 9 to a board. we then mounted a small 1/2 thick plank to the top of the taller one, such that we could attach the esp32 to it and test the camera fov. (insert taped picture of esp32) After a few tests we determined around 14 in would be the perfect height for the camera.

We found a black piece of construction paper, used to have a dark background for training images.

After this, we tried 3d printing a mount for the esp32 as well as the tof sensors. Austin designed a 2 part mounting system, however the scale of the appartus was to large so we decieded to only print the top portion. although it seemed promising, we quickly relaized tha tmounting it would be difficult as well as we needed ajdustability for the camera. we did not have enough time to prototype ultple iterations, so we took a different appraoch.  (insert 3 cad pictures)

We then transitioned to the CICS makerspace, and found the large plank to be slightly too tall when screwed on the top instead of the side. because of this, we decided to saw some off of the top of it, then firmilty mount it to the top of the base board. Finally, we firmly attached all parts and drilled a hole in the top plankf ro the camera to view through.

Once drilled, we had to figure out a method to standardize the postion of the esp32 as well as our tof sensors. We iterated through 4 versions of our top mount, fist starting with jus the mounting holes for the TOF sensors, finding an angle that works scun that they did not have noise from the hand detection while looking for gestures. We ended up on an angle of around 8 degrees up for each of the sneosr,s with poles to screw into for the tof sensors. we also include arms to guide the weight of the esp3e2 such that it is in a constnat place on the top for camera reference. this tightlly slid onto the wood base, with spots for 3 tof (insert 3d prints and such images, spots for 3 pictures)

in final assmebly, we realized that some of the tof mounts borke in the middle, which emans w trainsito to only 2 tof mounts.


To help our inference model, we decideied to paint everything a matte black, opting for the darkest backgtrouind possouibel to eliminate any noise. 
(insert paint pictures)

Lastly, because the serial clock was broken on our esp32 s3 cam that we have been using this entire time, we were forced to attach a makerboard we recieve from class to attach all the tof sensors.  (insert final 2 pictures)