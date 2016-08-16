CCV = ~/.gitaclib/ccv
LA_DEPS = $(CCV)/ccv.o

include $(shell sed '1q;d' ~/.libaldaron)/src/ProjectMakefile.mk

gitaclib: $(LA_DEPS)

$(CCV)/ccv.o:
	mkdir -p $(CCV)/ && \
	cd $(CCV)/ && \
#	wget https://github.com/liuliu/ccv/zipball/stable && \
#	mv * download.zip && unzip *.zip && \
#	rm *.zip && mv * ccv && \
	cd ccv/lib/ && \
	gcc ccv_cache.c -o ccv_cache.o -c && \
	gcc ccv_memory.c -o ccv_memory.o -c && \
	gcc ccv_resample.c -o ccv_resample.o -c && \
	gcc ccv_numeric.c -o ccv_numeric.o -c && \
	gcc ccv_basic.c -o ccv_basic.o -c && \
	gcc -c ccv_io.c -o ccv_io.o && \
	gcc -c ccv_algebra.c -o ccv_algebra.o && \
	gcc -c ccv_util.c -o ccv_util.o && \
	gcc -c ccv_image_processing.c -o ccv_image_processing.o && \
	gcc -c ccv_transform.c -o ccv_transform.o && \
	gcc -c ccv_classic.c -o ccv_classic.o && \
	gcc -c ccv_daisy.c -o ccv_daisy.o && \
	gcc -c ccv_sift.c -o ccv_sift.o && \
	gcc -c ccv_bbf.c -o ccv_bbf.o && \
	gcc -c ccv_mser.c -o ccv_mser.o && \
	gcc -c ccv_swt.c -o ccv_swt.o && \
	gcc -c ccv_dpm.c -o ccv_dpm.o && \
	gcc -c ccv_tld.c -o ccv_tld.o && \
	gcc -c ccv_ferns.c -o ccv_ferns.o && \
	gcc -c ccv_icf.c -o ccv_icf.o && \
	gcc -c ccv_scd.c -o ccv_scd.o -O3 -ffast-math && \
	gcc -c ccv_convnet.c -o ccv_convnet.o && \
	gcc -c ccv_output.c -o ccv_output.o && \
	gcc -c 3rdparty/sha1/sha1.c -o 3rdparty/sha1/sha1.o && \
	gcc -c 3rdparty/kissfft/kiss_fft.c -o 3rdparty/kissfft/kiss_fft.o && \
	gcc -c 3rdparty/kissfft/kiss_fftnd.c -o 3rdparty/kissfft/kiss_fftnd.o && \
	gcc -c 3rdparty/kissfft/kiss_fftr.c -o 3rdparty/kissfft/kiss_fftr.o && \
	gcc -c 3rdparty/kissfft/kiss_fftndr.c -o 3rdparty/kissfft/kiss_fftndr.o && \
	gcc -c 3rdparty/kissfft/kissf_fft.c -o 3rdparty/kissfft/kissf_fft.o && \
	gcc -c 3rdparty/kissfft/kissf_fftnd.c -o 3rdparty/kissfft/kissf_fftnd.o && \
	gcc -c 3rdparty/kissfft/kissf_fftr.c -o 3rdparty/kissfft/kissf_fftr.o && \
	gcc -c 3rdparty/kissfft/kissf_fftndr.c -o 3rdparty/kissfft/kissf_fftndr.o && \
	gcc -c 3rdparty/dsfmt/dSFMT.c -o 3rdparty/dsfmt/dSFMT.o && \
	gcc -c 3rdparty/sfmt/SFMT.c -o 3rdparty/sfmt/SFMT.o && \
	gcc -c 3rdparty/sqlite3/sqlite3.c -o 3rdparty/sqlite3/sqlite3.o && \
	ld -r `find . -type f -name '*.o'` -o ~/.gitaclib/ccv/ccv.o

