package com.example.sphere

interface Platform {
    val name: String
}

object CurrentPlatform : Platform {
    override val name: String = "Sphere Studio Multiplatform"
}
