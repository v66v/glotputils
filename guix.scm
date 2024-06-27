(use-modules (guix)
             (guix build-system gnu)
             (guix git-download)
             ;; (gnu packages pkg-config)
             ;; (gnu packages gcc)
             ;; (gnu packages guile)
             (gnu packages commencement)
             (gnu packages autotools)
             ;; (gnu packages base)
             (gnu packages)
			 (gnu packages xorg)
			 (gnu packages image)
             ((guix licenses)
              #:prefix license:))

(define vcs-file?
  ;; Return true if the given file is under version control.
  (or (git-predicate (current-source-directory))
      (const #t)))

(define plotutils
  (package
    (name "plotutils")
    (version "2.6")
    (source (origin
              (method url-fetch)
              (uri (string-append "file://" (current-source-directory)))
              (sha256
               (base32
                "1arkyizn5wbgvbh53aziv3s6lmd3wm9lqzkhxb3hijlp1y124hjg"))
              (modules '((guix build utils)))
              (snippet
               ;; Force the use of libXaw7 instead of libXaw.  When not doing
               ;; that, libplot.la ends up containing just "-lXaw" (without
               ;; "-L/path/to/Xaw"), due to the fact that there is no
               ;; libXaw.la, which forces us to propagate libXaw.
               '(begin
                  (substitute* "configure"
                    (("-lXaw")
                     "-lXaw7"))
                  ;; Use the `png_jmpbuf' accessor, as recommended since libpng
                  ;; 1.4.0 (see:
                  ;; http://www.libpng.org/pub/png/src/libpng-1.2.x-to-1.4.x-summary.txt).
                  (substitute* "libplot/z_write.c"
                    (("png_ptr->jmpbuf")
                     "png_jmpbuf (png_ptr)"))
                  #t))
              (patches
               ;; The test suite fails on some architectures such as i686 (see:
               ;; https://lists.gnu.org/archive/html/bug-plotutils/2016-04/msg00002.html).
               ;; The following Debian patch works around it.
               (search-patches "plotutils-spline-test.patch"))))
    (build-system gnu-build-system)
    (arguments
     (list #:configure-flags
           #~(list "--enable-libplotter"

                   ;; On i686 some tests fail due to excess floating point
                   ;; precision; work around it.  However, libplotter is C++
                   ;; and thus unaffected by CFLAGS, but '-fexcess-precision'
                   ;; is not implemented for C++ as of GCC 10.
                   #$@(if (target-x86-32?)
                          #~("CFLAGS=-g -O2 -fexcess-precision=standard")
                          #~()))

           #:phases
           (if (target-x86-32?)
               #~(modify-phases %standard-phases
                   (add-before 'check 'skip-sloppy-test
                     (lambda _
                       ;; This test reveals a slight difference in the SVG
                       ;; output due to floating point inequalities.  Skip it.
                       (substitute* "test/plot2svg.test"
                         (("^exit .*") "exit 77")))))
               #~%standard-phases)))
    (inputs (list libpng libx11 libxt libxaw gcc-toolchain automake autoconf libtool))
    (home-page "https://www.gnu.org/software/plotutils/")
    (synopsis "Plotting utilities and library")
    (description
     "GNU Plotutils is a package for plotting and working with 2D graphics.
It includes the C library @code{libplot} and the C++ @code{libplotter} library
for exporting 2D vector graphics in many file formats.  It also has support
for 2D vector graphics animations.  The package also contains command-line
programs for plotting scientific data.")
    (license license:gpl2+)))

plotutils
