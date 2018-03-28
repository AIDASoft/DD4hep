        SET(LATEX_OUTPUT_PATH ${CMAKE_BINARY_DIR}/DD4hepManual)
        ADD_LATEX_DOCUMENT(
                DD4hepManual.tex

                BIBFILES
                references.bib
                USE_BIBLATEX

                IMAGE_DIRS
                figures

                INPUTS
                config.tex
                chapters/basics.tex
                chapters/overview.tex
                figures/logo.eps

                html/dd4hep.cfg
                html/tools.sty
                html/js/fw.js
                html/js/sidebar.js
                html/css/custom.css

                EXCLUDE_FROM_ALL)