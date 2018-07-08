all: clip_prince
	cp *.c ..
	cp *.h ..

%: %.bmp
	python generate_clips.py $@_complete $@

clean:
	rm -rf *.c *.h
