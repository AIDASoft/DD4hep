(while argv
  (find-file (pop argv))
  (mark-whole-buffer)
  (set-buffer-file-coding-system 'unix 't)
  (setq-default c-default-style "ellemtel")
  (setq         c-set-style     "ellemtel")
  (setq         c-basic-offset  2)

  (indent-region (point-min) (point-max) nil)
  (delete-trailing-whitespace)
  (untabify (point-min) (point-max))
  (save-buffer))

(kill-emacs 0)
