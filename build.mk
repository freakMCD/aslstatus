SANITIZE := -fsanitize-address-use-after-scope \
	    -fsanitize=leak -fsanitize=undefined \
	    -fno-omit-frame-pointer -fno-optimize-sibling-calls \
	    -fsanitize=address,undefined,pointer-subtract,pointer-compare

DEBUG_CFLAGS  := -ggdb -g3 -O0 ${SANITIZE}
DEBUG_LDFLAGS := ${SANITIZE}

RELEASE_CFLAGS   := -march=native -O3 -flto -fPIC -fPIE \
		    -freciprocal-math -fstack-protector-strong \
		    -ftree-vectorize -funroll-loops -fno-math-errno \
		    -fno-trapping-math -fno-math-errno -ffinite-math-only \
		    -fno-rounding-math -fno-signed-zeros -fassociative-math
RELEASE_LDFLAGS  := -Wl,-O4 -Wl,--as-needed -Wl,-z,now -Wl,-z,relro
RELEASE_CPPFLAGS := -D_FORTIFY_SOURCE=2


.PHONY: debug
debug: all
debug: CFLAGS  += ${DEBUG_CFLAGS}
debug: LDFLAGS += ${DEBUG_LDFLAGS}

.PHONY: release
release: all
release: CFLAGS   += ${RELEASE_CFLAGS}
release: LDFLAGS  += ${RELEASE_LDFLAGS}
release: CPPFLAGS += ${RELEASE_CPPFLAGS}
