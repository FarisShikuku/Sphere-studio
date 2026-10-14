package com.example.sphere

class DesktopPlatform : Platform {
    override val name: String = "Desktop JVM"
}

fun getDesktopPlatform(): Platform = DesktopPlatform()
