VERSION = 3.2.2
DEFINES += VERSION='\\"$${VERSION}\\"'

exists(/usr/include/ukuisdk/kylin-com4cxx.h) {
    message("kylin common for cxx find.")
    DEFINES += KYLIN_COMMON=true
    LIBS += -lukui-com4cxx
}
