
        SET(LATEX_OUTPUT_PATH ${CMAKE_BINARY_DIR}/DDCondManual)
        ADD_LATEX_DOCUMENT(
                DDCondManual.tex
                IMAGE_DIRS
                figs
                setup

                INPUTS
                setup/AIDA2020-setup.tex
                setup/DD4hep-setup.tex

                html/dd4hep.cfg
                html/tools.sty
                html/js/fw.js
                html/js/sidebar.js
                html/css/custom.css

                EXCLUDE_FROM_ALL)
