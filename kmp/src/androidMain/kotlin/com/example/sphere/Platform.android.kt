package com.example.sphere

class AndroidPlatform : Platform {
    override val name: String = "Android Platform"
}

fun getAndroidPlatform(): Platform = AndroidPlatform()
