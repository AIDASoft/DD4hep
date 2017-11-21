
        SET(LATEX_OUTPUT_PATH ${CMAKE_BINARY_DIR}/DDG4Manual)
        ADD_LATEX_DOCUMENT(
                DDG4Manual.tex
                IMAGE_DIRS
                figs
                setup

                INPUTS
                Bibliography.tex
                sections/Components.tex
                sections/HighLevel.tex
                sections/Implementation.tex
                sections/Introduction.tex
                sections/MT.tex
                sections/Setup.tex
                setup/AIDA2020-setup.tex
                setup/DD4hep-setup.tex

                html/dd4hep.cfg
                html/tools.sty
                html/js/fw.js
                html/js/sidebar.js
                html/css/custom.css

                EXCLUDE_FROM_ALL)