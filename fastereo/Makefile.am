bin_PROGRAMS = fastereo
fastereo_SOURCES = fastereo.c \
		   commands.c \
		   cameras.c \
		   display.c \
		   geom.c \
		   utils.c \
		   img.c
fastereo_LDADD = xtdio.a
fastereo_LDADD += @GLUT_LIBS@
fastereo_LDADD += @GL_LIBS@

DOXYGEN = @DOXYGEN@
DOXYCONFIG = doxygen.config
doc: *.c *.h $(DOXYCONFIG)
	$(DOXYGEN) $(DOXYCONFIG)
