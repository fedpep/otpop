all: clip_prince
	cp *.c ..
	cp *.h ..


%: %_complete.bmp
	python generate_clips.py $@_complete $@

clean:
	rm -rf *.c *.h
