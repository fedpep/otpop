all: clip_prince clip_guard
	cp *.c ..
	cp *.h ..


%: %_complete.bmp
	python generate_clips.py $@_complete $@

clean:
	rm -rf *.c *.h clip_prince.bmp clip_guard.bmp
