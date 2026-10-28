package com.example.sphere.text.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.FontTypeface
import com.example.sphere.model.TextAlignment
import com.example.sphere.model.TextPreset

private val textPaletteColors = listOf(
    Color(0xFF2BF69A) to "Matrix Green",
    Color(0xFF29DCFF) to "Neon Cyan",
    Color(0xFFFFD56B) to "Warm Gold",
    Color(0xFFFF4838) to "Alert Crimson",
    Color(0xFFF2F5F8) to "Pure White",
    Color(0xFFBA68C8) to "Electric Violet",
    Color(0xFFFFB74D) to "Amber Solar"
)

@Composable
fun TypographyContentTab(
    text: String,
    onTextChange: (String) -> Unit,
    font: FontTypeface,
    onFontChange: (FontTypeface) -> Unit,
    fontSize: Float,
    onFontSizeChange: (Float) -> Unit,
    letterSpacing: Float,
    onLetterSpacingChange: (Float) -> Unit,
    lineHeight: Float,
    onLineHeightChange: (Float) -> Unit,
    alignment: TextAlignment,
    onAlignmentChange: (TextAlignment) -> Unit,
    textColor: Color,
    onTextColorChange: (Color) -> Unit,
    availablePresets: List<TextPreset>,
    onSelectPreset: (TextPreset) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp)
    ) {
        // Preset Quick Selector
        Text(
            "TYPOGRAPHY PRESETS",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(6.dp))
        LazyRow(
            horizontalArrangement = Arrangement.spacedBy(8.dp),
            modifier = Modifier.fillMaxWidth()
        ) {
            items(availablePresets) { preset ->
                FilterChip(
                    selected = false,
                    onClick = { onSelectPreset(preset) },
                    label = {
                        Text(
                            preset.name,
                            fontSize = 11.sp,
                            maxLines = 1
                        )
                    },
                    leadingIcon = {
                        Icon(
                            Icons.Default.TextFields,
                            contentDescription = null,
                            modifier = Modifier.size(14.dp)
                        )
                    },
                    modifier = Modifier.testTag("preset_chip_${preset.id}")
                )
            }
        }

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // Text Content Input Field
        Text(
            "TEXT CONTENT",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(6.dp))
        OutlinedTextField(
            value = text,
            onValueChange = onTextChange,
            modifier = Modifier
                .fillMaxWidth()
                .testTag("text_input_field"),
            minLines = 2,
            maxLines = 5,
            trailingIcon = {
                if (text.isNotEmpty()) {
                    IconButton(onClick = { onTextChange("") }) {
                        Icon(Icons.Default.Clear, contentDescription = "Clear Text")
                    }
                }
            },
            colors = OutlinedTextFieldDefaults.colors(
                focusedContainerColor = MaterialTheme.colorScheme.surface,
                unfocusedContainerColor = MaterialTheme.colorScheme.surface
            )
        )

        Spacer(Modifier.height(16.dp))

        // Font Typeface Selector
        Text(
            "FONT TYPEFACE",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
            FontTypeface.entries.forEach { f ->
                val isSelected = font == f
                Card(
                    onClick = { onFontChange(f) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.primaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
                    ),
                    modifier = Modifier.fillMaxWidth().testTag("font_card_${f.name}")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 10.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(selected = isSelected, onClick = { onFontChange(f) })
                        Spacer(Modifier.width(8.dp))
                        Column {
                            Text(
                                f.label,
                                style = MaterialTheme.typography.bodyMedium.copy(
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal,
                                    fontFamily = f.family
                                )
                            )
                            Text(
                                f.styleDesc,
                                style = MaterialTheme.typography.labelSmall.copy(
                                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                                    fontSize = 10.sp
                                )
                            )
                        }
                    }
                }
            }
        }

        Spacer(Modifier.height(16.dp))

        // Text Alignment Controls
        Text(
            "ALIGNMENT",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(6.dp))
        SingleChoiceSegmentedButtonRow(modifier = Modifier.fillMaxWidth()) {
            TextAlignment.entries.forEachIndexed { index, align ->
                SegmentedButton(
                    selected = alignment == align,
                    onClick = { onAlignmentChange(align) },
                    shape = SegmentedButtonDefaults.itemShape(index = index, count = TextAlignment.entries.size),
                    icon = {}
                ) {
                    val icon = when (align) {
                        TextAlignment.LEFT -> Icons.Default.FormatAlignLeft
                        TextAlignment.CENTER -> Icons.Default.FormatAlignCenter
                        TextAlignment.RIGHT -> Icons.Default.FormatAlignRight
                    }
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(icon, contentDescription = align.label, modifier = Modifier.size(16.dp))
                        Spacer(Modifier.width(4.dp))
                        Text(align.label, fontSize = 12.sp)
                    }
                }
            }
        }

        Spacer(Modifier.height(16.dp))

        // Primary Text Color Swatches
        Text(
            "TEXT COLOR",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        LazyRow(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
            items(textPaletteColors) { (col, name) ->
                val isSelected = textColor == col
                Box(
                    modifier = Modifier
                        .size(36.dp)
                        .clip(CircleShape)
                        .background(col)
                        .clickable { onTextColorChange(col) }
                        .then(
                            if (isSelected) Modifier.border(3.dp, MaterialTheme.colorScheme.onSurface, CircleShape)
                            else Modifier.border(1.dp, Color.Gray, CircleShape)
                        ),
                    contentAlignment = Alignment.Center
                ) {
                    if (isSelected) {
                        Icon(
                            Icons.Default.Check,
                            contentDescription = name,
                            tint = if (col == Color(0xFFF2F5F8) || col == Color(0xFFFFD56B)) Color.Black else Color.White,
                            modifier = Modifier.size(18.dp)
                        )
                    }
                }
            }
        }

        Spacer(Modifier.height(16.dp))

        // Font Size Slider
        Text(
            "Font Size: ${fontSize.toInt()} sp",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = fontSize,
            onValueChange = onFontSizeChange,
            valueRange = 12f..56f,
            modifier = Modifier.testTag("font_size_slider")
        )

        Spacer(Modifier.height(8.dp))

        // Letter Spacing Slider
        Text(
            "Letter Spacing: ${"%.2f".format(letterSpacing)}x",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = letterSpacing,
            onValueChange = onLetterSpacingChange,
            valueRange = 0.85f..1.80f,
            modifier = Modifier.testTag("letter_spacing_slider")
        )

        Spacer(Modifier.height(8.dp))

        // Line Height Slider
        Text(
            "Line Height: ${"%.2f".format(lineHeight)}x",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = lineHeight,
            onValueChange = onLineHeightChange,
            valueRange = 1.0f..2.0f,
            modifier = Modifier.testTag("line_height_slider")
        )
    }
}
