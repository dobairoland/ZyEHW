#TEST DATA

1. Take some videos, rescale and crop them to 128x128.

        ffmpeg -i videoin1.y4m -vf "crop=w=in_h,scale=128:128" video1.y4m

2. Concatenate the videos.

        ffmpeg -i video1.y4m -i video2.y4m -i video3.y4m \
                -filter_complex '[0:0] [1:0] [2:0] concat=n=3:v=1:a=0 [v]' \
                -map '[v]' videoconcat.y4m

3. Generate the test data based on the concatenated video.

        ./testvid -i videoconcat.y4m -o video.dat -sp 5

   or (for edge detection without noise)

        ./testvid -i videoconcat.y4m -o video.dat

   video.dat.y4m will be also generated to check the test data

4. Check the fitness values in the output XML.

        ./check -i video.dat -f filter.xml -med

   or

        ./check -i video.dat -f filter.xml -edge

#DEMONSTRATION

1. Take the same videos as before and change the resolution and aspect ratio
   to some common values, for example 16:9 and 240p.

        ffmpeg -i videoin1.y4m -aspect 16:9 video1.y4m
   or
        ffmpeg -i videoin2.y4m -vf scale=427x240 video2.y4m
   or
        ffmpeg -i videoin3.y4m -vf
        "crop=h=in_w*9/16,scale=427:240,setsar=sar=1/1" video3.y4m

2. Concatenate the videos.

        ffmpeg -i video1.y4m -i video2.y4m -i video3.y4m \
                -filter_complex '[0:0] [1:0] [2:0] concat=n=3:v=1:a=0 [v]' \
                -map '[v]' videoconcat.y4m

3. Generate the demonstration video using the concatenated video with the same
   noise as for the test data.

        ./vidprocsim -i videoconcat.y4m -f filter.xml -o demonstration.y4m \
                -sp 5 -med -d /tmp/dir

   or "-edge" instead of "-sp 5 -med" for edge detection.

4. Encode it for faster playback.

        ffmpeg -i demonstration.y4m demonstration.mp4
