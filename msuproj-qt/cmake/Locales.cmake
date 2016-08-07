#### The list of supported locales
set(MSUPROJ_QT_LOCALES
    en
    fr_FR
    hu_HU
    ru_RU)

#### The list Doxygen interface languages for supported locales
#### (http://www.stack.nl/~dimitri/doxygen/manual/langhowto.html)
set(en_LANG      English)
set(fr_FR_LANG   French)
set(hu_HU_LANG   Hungarian)
set(ru_RU_LANG   Russian)

#### Number of locales
list(LENGTH MSUPROJ_QT_LOCALES MSUPROJ_QT_LOCALES_SIZE)
