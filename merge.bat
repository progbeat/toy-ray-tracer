@ffmpeg -f image2 -i "out2/frame%04d.ppm" -b:v 20000k video.mpg
@ffmpeg -f image2 -i "mike/frame%04d.ppm" -vcodec libx264 -b:v 20000k 2mike_v2.avi
@pause