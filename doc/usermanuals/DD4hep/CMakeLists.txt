        SET(LATEX_OUTPUT_PATH ${CMAKE_BINARY_DIR}/DD4hepManual)
        ADD_LATEX_DOCUMENT(
                DD4hepManual.tex
                IMAGE_DIRS
                figs
                setup

                INPUTS
                sections/Abstract.tex
                sections/Overview.tex
                setup/AIDA2020-setup.tex
                setup/DD4hep-setup.tex
                Bibliography.tex

                html/dd4hep.cfg
                html/tools.sty
                html/js/fw.js
                html/js/sidebar.js
                html/css/custom.css

                EXCLUDE_FROM_ALL)