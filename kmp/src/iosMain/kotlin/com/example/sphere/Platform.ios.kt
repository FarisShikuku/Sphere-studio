package com.example.sphere

class IosPlatform : Platform {
    override val name: String = "iOS"
}

fun getIosPlatform(): Platform = IosPlatform()
