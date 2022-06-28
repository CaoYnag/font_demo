## Characters
|:---:|:---:|
|囧|56e7|
## ft1
freetype demo1

## ft2
freetype demo2
render a character with specified font to a picture.
usage:

```shell
# ft2 [font] [hex code] [output]
# in build dir
./ft2/ft2 fonts/luculent.ttf 42 test.png # got `B` picture
```

## ft3
freetype demo3
load all font face in single file, and render specified character into a picture.
usage:
```shell
# ft3 [font] [code] [output]
# sample
./ft3/ft3 fonts/wqy-zenhei.ttc 56e7 test
# then got serveral test_xx.png
```

## ft4
more detail about font face and glyph.

a sample output with char size **32**:
```shell
loaded nums: 1, fam: Luculent, style: Regular
FACE INFO:
num face: 1, current face idx: 0 fam: Luculent style: Regular
num glyph: 649 bbox: (0, -480, 1120, 2080) # 0, -7.5, 17.5, 32.5
height: 2560 # 40

GLYPH INFO:
advance: (1152, 0) # 18, 0
metrics size(768, 1600), hor (192 1600 1152) vert(-384 192 2112)
#             12,25           3   25   18         6    3   33

BITMAP INFO:
origin (3, 25)
size: (12, 25) pitch 12 num grey: 256
```