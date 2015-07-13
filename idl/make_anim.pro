pro make_anim,list,mpegname=mpegname,gifname=gifname,scale=scale,display=display,PNGNAME=pngname
  
  frame=readlist(list)
  
  image=read_tiff(frame[0])
  xsize=n_elements(image[0,*,0])
  ysize=n_elements(image[0,0,*])
  if keyword_set(scale) then begin
      xsize=ceil(scale*xsize)
      ysize=ceil(scale*ysize)
  endif

  if keyword_set(mpegname) then mpegID = MPEG_OPEN([xsize,ysize])
  
  for i=0,n_elements(frame)-1 do begin
      print,'Frame ',i
    image=read_tiff(frame[i])
   
    if keyword_set(scale) then image=congrid(image,3,xsize,ysize)
    
    if keyword_set(pngname) then write_png,pngname+padstring(string(format='(I2)',i))+'.png',image

    r=bytarr(xsize,ysize)
    g=bytarr(xsize,ysize)
    b=bytarr(xsize,ysize)
    r[*,*]=image[0,*,*]
    g[*,*]=image[1,*,*]
    b[*,*]=image[2,*,*]
    image[0,*,*]=rotate(r,7)
    image[1,*,*]=rotate(g,7)
    image[2,*,*]=rotate(b,7)

    if keyword_set(mpegname) then MPEG_PUT, mpegID, IMAGE=image,frame=i,/order
    if keyword_set(gifname) then begin
        gifimage=COLOR_QUAN(image, 1, red, green, blue, CUBE=6)
        write_gif,gifname,gifimage,red,green,blue,/multiple
    endif
    if keyword_set(display) then tv,image,true=1
  endfor

  if keyword_set(mpegname) then begin	
      print,'Writing MPEG file ....'	
      MPEG_SAVE, mpegID, FILENAME=mpegname
      MPEG_CLOSE, mpegID
  endif

  if keyword_set(pngname) then begin	
      print,'Wrote PNG frames ....'	
  endif

  if keyword_set(gifname) then begin
      print,'Written ANIMGIF file ....'	
      write_gif,/close
  endif    

end
