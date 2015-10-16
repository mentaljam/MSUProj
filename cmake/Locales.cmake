################### Documentation Locales ##################

#### The list of supported locales
set(DOXY_LOCALES en
                 fr_FR
                 hu_HU
                 ru_RU
)

#### The list Doxygen interface languages for supported locales
#### (http://www.stack.nl/~dimitri/doxygen/manual/langhowto.html)
set(en_LANG      English)
set(fr_FR_LANG   French)
set(hu_HU_LANG   Hungarian)
set(ru_RU_LANG   Russian)

#### Number of locales
list(LENGTH DOXY_LOCALES DOXY_LOCALES_SIZE)
