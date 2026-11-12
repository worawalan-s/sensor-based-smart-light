# Smart Lighting System

Automatic hallway and room illumination system activated conditionally by ambient darkness and obstacle proximity.

## Language

**Ambient Darkness**:
A condition where measured ambient illuminance falls below the calibrated activation threshold.
_Avoid_: Night mode, dark state

**Obstacle Presence**:
A discrete signal indicating an entity or object within the optical detection distance of the active infrared sensor.
_Avoid_: Motion, movement trigger

**Light Lockout**:
A protective state that suspends ambient light evaluation while the primary illumination load is energized, avoiding optical oscillation.
_Avoid_: Sensor masking, blind mode

**Hold Duration**:
The time interval the illumination load remains continuously energized after the latest qualified trigger event before automatic cutoff.
_Avoid_: Delay time, sleep time

**Remote Override**:
An external command from an operator or networked interface that forces the illumination load on or off independently of local sensor triggers.
_Avoid_: Manual force, Wi-Fi switch
