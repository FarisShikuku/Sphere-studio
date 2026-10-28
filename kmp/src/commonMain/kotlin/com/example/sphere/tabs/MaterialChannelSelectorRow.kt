package com.example.sphere.tabs

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.MaterialChannelMode

@Composable
fun MaterialChannelSelectorRow(
    selectedChannel: MaterialChannelMode,
    onChannelSelect: (MaterialChannelMode) -> Unit,
    modifier: Modifier = Modifier
) {
    Row(
        modifier = modifier
            .fillMaxWidth()
            .horizontalScroll(rememberScrollState())
            .padding(horizontal = 12.dp, vertical = 4.dp),
        horizontalArrangement = Arrangement.spacedBy(6.dp)
    ) {
        MaterialChannelMode.entries.forEach { mode ->
            val isSelected = mode == selectedChannel
            FilterChip(
                selected = isSelected,
                onClick = { onChannelSelect(mode) },
                label = { Text(mode.label, fontSize = 11.sp) },
                modifier = Modifier.testTag("channel_${mode.name.lowercase()}"),
                colors = FilterChipDefaults.filterChipColors(
                    selectedContainerColor = Color(0xFF0284C7).copy(alpha = 0.35f),
                    selectedLabelColor = Color(0xFF38BDF8),
                    containerColor = Color(0xFF131722),
                    labelColor = Color(0xFF94A3B8)
                )
            )
        }
    }
}
