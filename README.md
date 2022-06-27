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