all: clip_prince clip_guard
	cp *.c ..
	cp *.h ..


%: %_complete.bmp %_indexes.py
	python generate_clips.py $@_complete $@ $@_indexes.py

clean:
	rm -rf *.c *.h *.pyc clip_prince.bmp clip_guard.bmp
