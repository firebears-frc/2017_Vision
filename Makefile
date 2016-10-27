VISION_CC = gcc -c -O2 -ffast-math -msse2 -D HAVE_SSE2 -D HAVE_GSL
LINKER_LIBS = -lpng -lgsl -lgslcblas
CCV = ~/.gitaclib/ccv
CCV_LIB = `pwd`/ccv/lib
LA_DEPS = $(CCV_LIB)/*.o $(CCV_LIB)/3rdparty/sfmt/SFMT.o $(CCV_LIB)/3rdparty/dsfmt/dSFMT.o $(CCV_LIB)/3rdparty/sha1/sha1.o $(CCV_LIB)/3rdparty/kissfft/*.o $(CCV_LIB)/3rdparty/sqlite3/sqlite3.o

include $(shell sed '1q;d' ~/.libaldaron)/src/ProjectMakefile.mk

LIB = $(CCV)/ccv/lib

gitaclib: $(LA_DEPS)

$(CCV)/ccv.o:
	mkdir -p $(CCV)/ && \
	cd $(CCV)/ && \
#	wget https://github.com/liuliu/ccv/zipball/stable && \
#	mv * download.zip && unzip *.zip && \
#	rm *.zip && mv * ccv && \
	$(VISION_CC) $(LIB)/ccv_cache.c -o ccv_cache.o
	$(VISION_CC) $(LIB)/ccv_memory.c -o ccv_memory.o
	$(VISION_CC) $(LIB)/ccv_resample.c -o ccv_resample.o
	$(VISION_CC) $(LIB)/ccv_numeric.c -o ccv_numeric.o
	$(VISION_CC) $(LIB)/ccv_basic.c -o ccv_basic.o
	$(VISION_CC) $(LIB)/ccv_io.c -o ccv_io.o
	$(VISION_CC) $(LIB)/ccv_algebra.c -o ccv_algebra.o
	$(VISION_CC) $(LIB)/ccv_util.c -o ccv_util.o
	$(VISION_CC) $(LIB)/ccv_image_processing.c -o ccv_image_processing.o
	$(VISION_CC) $(LIB)/ccv_transform.c -o ccv_transform.o
	$(VISION_CC) $(LIB)/ccv_classic.c -o ccv_classic.o
	$(VISION_CC) $(LIB)/ccv_daisy.c -o ccv_daisy.o
	$(VISION_CC) $(LIB)/ccv_sift.c -o ccv_sift.o
	$(VISION_CC) $(LIB)/ccv_bbf.c -o ccv_bbf.o
	$(VISION_CC) $(LIB)/ccv_mser.c -o ccv_mser.o
	$(VISION_CC) $(LIB)/ccv_swt.c -o ccv_swt.o
	$(VISION_CC) $(LIB)/ccv_dpm.c -o ccv_dpm.o
	$(VISION_CC) $(LIB)/ccv_tld.c -o ccv_tld.o
	$(VISION_CC) $(LIB)/ccv_ferns.c -o ccv_ferns.o
	$(VISION_CC) $(LIB)/ccv_icf.c -o ccv_icf.o
	$(VISION_CC) $(LIB)/ccv_scd.c -o ccv_scd.o -O3 -ffast-math
	$(VISION_CC) $(LIB)/ccv_convnet.c -o ccv_convnet.o
	$(VISION_CC) $(LIB)/ccv_output.c -o ccv_output.o
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

