// Not Sure Yet

#pragma once

/**
 * Custom collision channels for Aura
 * These provide compile-time safety for collision traces
 * 
 * Setup: In Project Settings > Engine > Collision, create a custom Trace Channel named "Navigation"
 * Set its default response to "Ignore", then configure your floor/navigable surfaces to Block it.
 */

// Custom trace channel for navigation queries
// Maps to the first custom trace channel (ECC_GameTraceChannel1)
#define ECC_Navigation ECC_GameTraceChannel1