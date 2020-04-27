#-------------------------------------------------
#
# Project created by QtCreator 2019-08-19T18:03:13
#
#-------------------------------------------------

QT       += core gui network quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCardManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += BC_STATIC

DEFINES += USE_NUM_NONE
DEFINES += USE_SCALAR_8X32
DEFINES += USE_FIELD_10X26
DEFINES += USE_FIELD_INV_BUILTIN
DEFINES += USE_SCALAR_INV_BUILTIN
DEFINES += HAVE_ROUND
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += WIN32_LEAN_AND_MEAN

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += VERSION_QML

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

LIBS += winscard.lib
LIBS += -L$$PWD/lib/openssl/lib -llibeay32
LIBS += -L$$PWD/lib/openssl/lib -lssleay32
LIBS += -L$$PWD/lib/libbitcoin -lblocklink_libbitcoin
LIBS += -L$$PWD/lib/libbitcoin -lblocklink_libbitcoin_secp256k1
LIBS += -L$$PWD/lib/libethereum -lcryptlib
#LIBS += -L$$PWD/lib/libethereum -leth_sign
LIBS += -L$$PWD/lib/leveldb -lleveldb
LIBS += -L$$PWD/lib/leveldb -lShLwApi

LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_system-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_thread-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_date_time-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_chrono-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_regex-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_filesystem-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_program_options-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_log-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_atomic-vc141-mt-1_64.lib
LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_locale-vc141-mt-1_64.lib
#LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_locale-vc141-mt-1_64.lib
#LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_locale-vc141-mt-1_64.lib
#LIBS += E:/boost_1_64_0/lib64-msvc-14.1/libboost_locale-vc141-mt-1_64.lib


INCLUDEPATH += $$PWD/lib/openssl/include/
INCLUDEPATH += $$PWD/lib/libbitcoin/include/
INCLUDEPATH += $$PWD/lib/libethereum/
#INCLUDEPATH += $$PWD/lib/libethereum/include/
INCLUDEPATH += E:/boost_1_64_0
INCLUDEPATH += $$PWD/lib/leveldb/include/
INCLUDEPATH += $$PWD/lib/secp256k1/include/
INCLUDEPATH += $$PWD/lib/secp256k1

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    GUIData.cpp \
    common/HttpManager.cpp \
    common/commondialog.cpp \
    lib/GP/GP_Command.cpp \
    lib/GP/GP_Delete.cpp \
    lib/GP/GP_ExtAuth.cpp \
    lib/GP/GP_GetData.cpp \
    lib/GP/GP_GetStatus.cpp \
    lib/GP/GP_InitUpd.cpp \
    lib/GP/GP_Install.cpp \
    lib/GP/GP_Load.cpp \
    lib/GP/GP_PutKey.cpp \
    lib/GP/GP_SecurityInfo.cpp \
    lib/GP/GP_Select.cpp \
    lib/GP/GP_SetStatus.cpp \
    lib/GP/GP_StoreData.cpp \
    util/SSLUtils.cpp \
    util/bitcoin/base58.cpp \
    util/bitcoin/BitcoinUtil.cpp \
    gui/multisig/AddPubKeyDialog.cpp \
    gui/multisig/CreateMultisigDialog.cpp \
    gui/multisig/MultiAddressWidget.cpp \
    gui/cardinfo/BroadcastTrxDialog.cpp \
    gui/cardinfo/CardInfoWidget.cpp \
    gui/cardinfo/SignBtcTrxDialog.cpp \
    gui/cardinfo/TransferBtcDialog.cpp \
    common/showcontentdialog.cpp \
    gui/cardinfo/SignMultisigTrxDialog.cpp \
    util/ethereum/EthereumUtil.cpp \
    util/ethereum/Keccak.cpp \
    lib/libethereum/libdevcore/Address.cpp \
    lib/libethereum/libdevcore/Base64.cpp \
    lib/libethereum/libdevcore/CommonData.cpp \
    lib/libethereum/libdevcore/CommonIO.cpp \
    lib/libethereum/libdevcore/DevCoreCommon.cpp \
    lib/libethereum/libdevcore/DevCoreCommonJS.cpp \
    lib/libethereum/libdevcore/FileSystem.cpp \
    lib/libethereum/libdevcore/FixedHash.cpp \
    lib/libethereum/libdevcore/Guards.cpp \
    lib/libethereum/libdevcore/LevelDB.cpp \
    lib/libethereum/libdevcore/Log.cpp \
    lib/libethereum/libdevcore/MemoryDB.cpp \
    lib/libethereum/libdevcore/OverlayDB.cpp \
    lib/libethereum/libdevcore/RLP.cpp \
    lib/libethereum/libdevcore/SHA3.cpp \
    lib/libethereum/libdevcore/TransientDirectory.cpp \
    lib/libethereum/libdevcore/TrieCommon.cpp \
    lib/libethereum/libdevcore/TrieHash.cpp \
    lib/libethereum/libdevcore/Worker.cpp \
    lib/libethereum/libdevcrypto/AES.cpp \
    lib/libethereum/libdevcrypto/CryptoPP.cpp \
    lib/libethereum/libdevcrypto/DevCryptoCommon.cpp \
    lib/libethereum/libdevcrypto/Hash.cpp \
    lib/libethereum/libethcore/BlockHeader.cpp \
    lib/libethereum/libethcore/ChainOperationParams.cpp \
    lib/libethereum/libethcore/Common.cpp \
    lib/libethereum/libethcore/CommonJS.cpp \
    lib/libethereum/libethcore/LogEntry.cpp \
    lib/libethereum/libethcore/Precompiled.cpp \
    lib/libethereum/libethcore/SealEngine.cpp \
    lib/libethereum/libethcore/TransactionBase.cpp \
    lib/secp256k1/src/eth_secp256k1.c \
    gui/cardinfo/TransferEthDialog.cpp \
    gui/multisig/CreateEthMultisigContractDialog.cpp \
    gui/multisig/EthContractTransferDialog.cpp \
    gui/multisig/EthMultisigInfoDialog.cpp \
    util/bitcoin/BCHUtil.cpp \
    util/bitcoin/cashaddr.cpp \
    model/CardModel.cpp \
    card/CardView.cpp \
    card/CSmartCard.cpp \
    model/MultiAddressesModel.cpp
#    lib/secp256k1/src/bench_ecdh.c \
#    lib/secp256k1/src/bench_internal.c \
#    lib/secp256k1/src/bench_recover.c \
#    lib/secp256k1/src/bench_schnorr_verify.c \
#    lib/secp256k1/src/bench_sign.c \
#    lib/secp256k1/src/bench_verify.c \
#    lib/secp256k1/src/eth_secp256k1.c \
#    lib/secp256k1/src/gen_context.c \
#    lib/secp256k1/src/tests.c \
#    lib/secp256k1/src/tests_exhaustive.c

HEADERS += \
        mainwindow.h \
    GUIData.h \
    common/HttpManager.h \
    common/commondialog.h \
    common/style.h \
    lib/GP/GP_Command.h \
    lib/GP/GP_Delete.h \
    lib/GP/GP_ExtAuth.h \
    lib/GP/GP_GetData.h \
    lib/GP/GP_GetStatus.h \
    lib/GP/GP_InitUpd.h \
    lib/GP/GP_Install.h \
    lib/GP/GP_ITransmitter.h \
    lib/GP/GP_Load.h \
    lib/GP/GP_PutKey.h \
    lib/GP/GP_SecurityInfo.h \
    lib/GP/GP_Select.h \
    lib/GP/GP_SetStatus.h \
    lib/GP/GP_StoreData.h \
    util/SSLUtils.h \
    util/bitcoin/base58.h \
    util/bitcoin/BitcoinUtil.h \
    gui/multisig/AddPubKeyDialog.h \
    gui/multisig/CreateMultisigDialog.h \
    gui/multisig/MultiAddressWidget.h \
    gui/cardinfo/BroadcastTrxDialog.h \
    gui/cardinfo/CardInfoWidget.h \
    gui/cardinfo/SignBtcTrxDialog.h \
    gui/cardinfo/TransferBtcDialog.h \
    common/showcontentdialog.h \
    gui/cardinfo/SignMultisigTrxDialog.h \
    gui/style/style.h \
    util/ethereum/EthereumUtil.h \
    util/ethereum/Keccak.hpp \
    lib/libethereum/libdevcore/Address.h \
    lib/libethereum/libdevcore/Assertions.h \
    lib/libethereum/libdevcore/Base64.h \
    lib/libethereum/libdevcore/CommonData.h \
    lib/libethereum/libdevcore/CommonIO.h \
    lib/libethereum/libdevcore/concurrent_queue.h \
    lib/libethereum/libdevcore/db.h \
    lib/libethereum/libdevcore/dbfwd.h \
    lib/libethereum/libdevcore/DBImpl.h \
    lib/libethereum/libdevcore/DevCoreCommon.h \
    lib/libethereum/libdevcore/DevCoreCommonJS.h \
    lib/libethereum/libdevcore/Exceptions.h \
    lib/libethereum/libdevcore/FileSystem.h \
    lib/libethereum/libdevcore/FixedHash.h \
    lib/libethereum/libdevcore/Guards.h \
    lib/libethereum/libdevcore/LevelDB.h \
    lib/libethereum/libdevcore/Log.h \
    lib/libethereum/libdevcore/MemoryDB.h \
    lib/libethereum/libdevcore/OverlayDB.h \
    lib/libethereum/libdevcore/RangeMask.h \
    lib/libethereum/libdevcore/RLP.h \
    lib/libethereum/libdevcore/SHA3.h \
    lib/libethereum/libdevcore/Terminal.h \
    lib/libethereum/libdevcore/TransientDirectory.h \
    lib/libethereum/libdevcore/TrieCommon.h \
    lib/libethereum/libdevcore/TrieDB.h \
    lib/libethereum/libdevcore/TrieHash.h \
    lib/libethereum/libdevcore/UndefMacros.h \
    lib/libethereum/libdevcore/vector_ref.h \
    lib/libethereum/libdevcore/Worker.h \
    lib/libethereum/libdevcrypto/AES.h \
    lib/libethereum/libdevcrypto/CryptoPP.h \
    lib/libethereum/libdevcrypto/DevCryptoCommon.h \
    lib/libethereum/libdevcrypto/Exceptions.h \
    lib/libethereum/libdevcrypto/Hash.h \
    lib/libethereum/libethcore/BlockHeader.h \
    lib/libethereum/libethcore/ChainOperationParams.h \
    lib/libethereum/libethcore/Common.h \
    lib/libethereum/libethcore/CommonJS.h \
    lib/libethereum/libethcore/EVMSchedule.h \
    lib/libethereum/libethcore/Exceptions.h \
    lib/libethereum/libethcore/LogEntry.h \
    lib/libethereum/libethcore/Precompiled.h \
    lib/libethereum/libethcore/SealEngine.h \
    lib/libethereum/libethcore/TransactionBase.h \
    gui/cardinfo/TransferEthDialog.h \
    gui/multisig/CreateEthMultisigContractDialog.h \
    gui/multisig/EthContractTransferDialog.h \
    gui/multisig/EthMultisigInfoDialog.h \
    util/bitcoin/BCHUtil.h \
    util/bitcoin/cashaddr.h \
    model/CardModel.h \
    card/CardView.h \
    card/CSmartCard.h \
    model/MultiAddressesModel.h
#    lib/secp256k1/src/basic-config.h \
#    lib/secp256k1/src/bench.h \
#    lib/secp256k1/src/ecdsa.h \
#    lib/secp256k1/src/ecdsa_impl.h \
#    lib/secp256k1/src/eckey.h \
#    lib/secp256k1/src/eckey_impl.h \
#    lib/secp256k1/src/ecmult.h \
#    lib/secp256k1/src/ecmult_const.h \
#    lib/secp256k1/src/ecmult_const_impl.h \
#    lib/secp256k1/src/ecmult_gen.h \
#    lib/secp256k1/src/ecmult_gen_impl.h \
#    lib/secp256k1/src/ecmult_impl.h \
#    lib/secp256k1/src/ecmult_static_context.h \
#    lib/secp256k1/src/field.h \
#    lib/secp256k1/src/field_5x52.h \
#    lib/secp256k1/src/field_5x52_asm_impl.h \
#    lib/secp256k1/src/field_5x52_impl.h \
#    lib/secp256k1/src/field_5x52_int128_impl.h \
#    lib/secp256k1/src/field_10x26.h \
#    lib/secp256k1/src/field_10x26_impl.h \
#    lib/secp256k1/src/field_impl.h \
#    lib/secp256k1/src/group.h \
#    lib/secp256k1/src/group_impl.h \
#    lib/secp256k1/src/hash_impl.h \
#    lib/secp256k1/src/num.h \
#    lib/secp256k1/src/num_gmp.h \
#    lib/secp256k1/src/num_gmp_impl.h \
#    lib/secp256k1/src/num_impl.h \
#    lib/secp256k1/src/scalar.h \
#    lib/secp256k1/src/scalar_4x64.h \
#    lib/secp256k1/src/scalar_4x64_impl.h \
#    lib/secp256k1/src/scalar_8x32.h \
#    lib/secp256k1/src/scalar_8x32_impl.h \
#    lib/secp256k1/src/scalar_impl.h \
#    lib/secp256k1/src/scalar_low.h \
#    lib/secp256k1/src/scalar_low_impl.h \
#    lib/secp256k1/src/testrand.h \
#    lib/secp256k1/src/testrand_impl.h \
#    lib/secp256k1/src/util.h

FORMS += \
        mainwindow.ui \
    common/commondialog.ui \
    gui/multisig/AddPubKeyDialog.ui \
    gui/multisig/CreateMultisigDialog.ui \
    gui/multisig/MultiAddressWidget.ui \
    gui/cardinfo/BroadcastTrxDialog.ui \
    gui/cardinfo/CardInfoWidget.ui \
    gui/cardinfo/SignBtcTrxDialog.ui \
    gui/cardinfo/TransferBtcDialog.ui \
    common/showcontentdialog.ui \
    gui/cardinfo/SignMultisigTrxDialog.ui \
    gui/cardinfo/TransferEthDialog.ui \
    gui/multisig/CreateEthMultisigContractDialog.ui \
    gui/multisig/EthContractTransferDialog.ui \
    gui/multisig/EthMultisigInfoDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    syle.qrc \
    qml.qrc

DISTFILES +=


