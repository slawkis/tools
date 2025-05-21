; GIMP - The GNU Image Manipulation Program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
;
(define (script-fu-crop-selection image drawable)
  (let* (
        (selection-bounds (gimp-selection-bounds image))
        (select-x1	(cadr  selection-bounds))
        (select-y1	(caddr selection-bounds))
        (select-x2	(cadr  (cddr selection-bounds)))
        (select-y2	(caddr (cddr selection-bounds)))
        (iWidth		(car (gimp-image-width image)))
        (iHeight	(car (gimp-image-height image)))
	(sWidth		(- select-x2 select-x1))
	(sHeight	(- select-y2 select-y1))
        (selY		0)
        (selH		0)
        (selX		0)
        (selW		0)
        (selYp		0)
        (selXp		0)
        (selHp		0)
        (selWp		0)
        (cropW		0)
        (cropH		0)
	)

    (if (and (= (car (gimp-selection-is-empty image)) FALSE) (not (eq? sWidth sHeight)) )
        (begin

	(if (> sWidth sHeight)
	    (begin
		(set! selY select-y2)
		(set! selH (- iHeight select-y2))
		(set! selX 0)
		(set! selW iWidth)
		(set! selYp select-y1)
		(set! selHp (- iHeight select-y2))
		(set! selXp 0)
		(set! selWp iWidth)
		(set! cropW iWidth)
		(set! cropH (+ (- iHeight select-y2) select-y1))
	    )
	    (begin
		(set! selX select-x2)
		(set! selW (- iWidth select-x2))
		(set! selY 0)
		(set! selH iHeight)         
		(set! selXp select-x1)
		(set! selWp (- iWidth select-x2))
		(set! selYp 0)
		(set! selHp iHeight)
		(set! cropW (+ (- iWidth select-x2) select-x1))
		(set! cropH iHeight)
	    )
	)
	
	(gimp-image-undo-group-start image)
	(gimp-selection-none image)
	(gimp-image-select-rectangle image CHANNEL-OP-REPLACE selX selY selW selH)
	
	(gimp-edit-copy drawable);

	(gimp-selection-none image);
	(gimp-image-select-rectangle image CHANNEL-OP-REPLACE selXp selYp selWp selHp)

	(gimp-edit-paste drawable TRUE)
	(gimp-floating-sel-anchor (car (gimp-image-get-floating-sel image)))

	(gimp-selection-none image)
	(gimp-image-crop image cropW cropH 0 0)

	(gimp-image-undo-group-end image)

	(gimp-displays-flush)
	)
    )
  )
)

(script-fu-register "script-fu-crop-selection"
  _"Remove selection"
  _"Crop out selection from image"
  "steev"
  "steev"
  "2024/09/25"
  "RGB* GRAY*"
  SF-IMAGE "Image"       0
  SF-DRAWABLE "Drawable" 0
)

(script-fu-menu-register "script-fu-crop-selection" "<Image>/Layer")
