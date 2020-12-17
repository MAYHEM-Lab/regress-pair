cmake_minimum_required(VERSION 3.13)
project(regress-pair)

find_package(woof REQUIRED)
find_package(matrix REQUIRED)

add_executable(regress-pair-put regress-pair-put.c)
target_link_libraries(regress-pair-put PRIVATE woof woofc-host)

add_executable(regress-pair-get regress-pair-get.c)
target_link_libraries(regress-pair-get PRIVATE woof woofc-host)

add_executable(regress-pair-init regress-pair-init.c)
target_link_libraries(regress-pair-init PRIVATE woof woofc-host)

add_executable(regress-pair-update regress-pair-update.c)
target_link_libraries(regress-pair-update PRIVATE woof woofc-host)

add_library(regress-matrix regress-matrix.c ssa-decomp.c)
target_link_libraries(regress-matrix PUBLIC mioarray mioregress)

add_handler(RegressPairReqHandler RegressPairReqHandler.c)

add_handler(RegressPairPredictedHandler RegressPairPredictedHandler.c)
target_link_libraries(RegressPairPredictedHandler PUBLIC regress-matrix)

add_handler(RegressPairMeasuredHandler RegressPairMeasuredHandler.c)
target_link_libraries(RegressPairMeasuredHandler PUBLIC regress-matrix)
