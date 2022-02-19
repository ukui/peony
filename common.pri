VERSION = 3.10.0
DEFINES += VERSION='\\"$${VERSION}\\"'

DEFINES += V10_SP1='\\"V10SP1\\"'
DEFINES += V10_SP1_EDU='\\"V10SP1-edu\\"'

exists(/usr/include/ukuisdk/kylin-com4cxx.h) {
    message("kylin common for cxx find.")
    DEFINES += KYLIN_COMMON=true
    LIBS += -lukui-com4cxx
}
