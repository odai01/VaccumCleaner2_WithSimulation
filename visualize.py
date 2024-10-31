import pygame
import json
import math

# Load data from the JSON files (steps history and the house matrix)
with open('steps_history.json') as f:
    steps_history = json.load(f)
with open('initial_house.json') as f:
    house_data = json.load(f)

house_matrix = house_data['house']
maxSteps = house_data['maxSteps']
maxBattery = house_data['maxBattery']

# Initialize Pygame
pygame.init()

rows = len(house_matrix)
cols = len(house_matrix[0])

# Display dimensions
block_size = 50  # If we want to enlargen the house we can increase block size
margin = 20
width = cols * block_size + 2 * margin
height = rows * block_size + 2 * margin

# Center the house on the screen
screen_width = max(width, 800)
screen_height = max(height, 600)
window = pygame.display.set_mode((screen_width, screen_height + 200))  # Extra space for legend and buttons
pygame.display.set_caption("Vacuum Cleaner Simulation")

# Colors we will use
MAROON = (128, 0, 0)
LIGHTYELLOW = (216, 191, 216) # it is light purple not light yellow
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GOLD = (255, 215, 0)
WALL_COLOR = (0, 0, 128)
GREEN = (107, 142, 35)
RED = (255, 99, 71)
BatteryBar =(50, 205, 50)
StepsBar=(64, 224, 208)

docking_station = steps_history[0] if steps_history else None

# Function to calculate gray color based on dirt level
def get_dirt_color(level):
    if level < 0 or level > 9:
        return WHITE
    gray_value = 255 - (level * 25)
    return (gray_value, gray_value, gray_value)

# Function to draw the house grid
def draw_grid(charging):
    start_x = (screen_width - width) // 2
    start_y = (screen_height - height) // 2
    for i in range(rows):
        for j in range(cols):
            if house_matrix[i][j] == -1:
                color = WALL_COLOR
            else:
                color = get_dirt_color(house_matrix[i][j])
            rect = pygame.Rect(start_x + j * block_size, start_y + i * block_size, block_size, block_size)
            pygame.draw.rect(window, color, rect)
            if house_matrix[i][j] > 0:
                font = pygame.font.SysFont(None, 24)
                text = font.render(str(house_matrix[i][j]), True, BLACK)
                window.blit(text, (start_x + j * block_size + block_size // 2 - text.get_width() // 2,
                                   start_y + i * block_size + block_size // 2 - text.get_height() // 2))
            if (i, j) == (docking_station['x'], docking_station['y']):
                draw_docking_station_symbol(start_x + j * block_size, start_y + i * block_size, charging)

# Function to draw the docking station symbol: the Plus sign
def draw_docking_station_symbol(x, y, charging):
    color = GOLD if charging else BLACK  # When charging it turns gold
    pygame.draw.line(window, color, (x + 7, y + 5), (x + 14, y + 5), 2)
    pygame.draw.line(window, color, (x + 10, y + 2), (x + 10, y + 9), 2)

# Function to draw the legend
def draw_legend():
    font = pygame.font.SysFont(None, 24)
    legend_items = [
        ("Wall", WALL_COLOR),
        ("Vacuum Cleaner", GOLD),
        ("Dirt Levels", BLACK),
        ("Docking Station", WHITE)
    ]
    x, y = margin, screen_height - 180
    for text, color in legend_items:
        if text == "Vacuum Cleaner":
            pygame.draw.circle(window, color, (x + block_size // 2, y + block_size // 2), block_size // 4)
            pygame.draw.line(window, MAROON,
                             (x + block_size // 2, y + block_size // 2),
                             (x + block_size // 2 + block_size // 6 * math.cos(math.radians(0)),
                              y + block_size // 2 + block_size // 6 * math.sin(math.radians(0))), 2)
            legend_text = font.render(text, True, BLACK)
            window.blit(legend_text, (x + block_size + 15, y + block_size // 2 - legend_text.get_height() // 2))
        elif text == "Dirt Levels":
            for level in range(10):
                dirt_color = get_dirt_color(level)
                pygame.draw.rect(window, dirt_color, (x + level * (block_size // 10), y, block_size // 10, block_size))
            legend_text = font.render(text, True, BLACK)
            window.blit(legend_text, (x + block_size + 15, y + block_size // 2 - legend_text.get_height() // 2))
        elif text == "Docking Station":
            pygame.draw.rect(window, WHITE, (x, y, block_size, block_size))
            draw_docking_station_symbol(x, y, False)  # Legend doesn't flash
            legend_text = font.render(text, True, BLACK)
            window.blit(legend_text, (x + block_size + 15, y + block_size // 2 - legend_text.get_height() // 2))
        else:
            pygame.draw.rect(window, color, (x, y, block_size, block_size))
            legend_text = font.render(text, True, BLACK)
            window.blit(legend_text, (x + block_size + 15, y + block_size // 2 - legend_text.get_height() // 2))
        y += block_size + 20

# Function to draw buttons
def draw_buttons():
    font = pygame.font.SysFont(None, 24)
    buttons = [
        ("Resume", GREEN, (screen_width // 2 - 100, screen_height + 40, 80, 40)),
        ("Pause", RED, (screen_width // 2 + 20, screen_height + 40, 80, 40))
    ]
    for text, color, rect in buttons:
        pygame.draw.rect(window, color, rect)
        button_text = font.render(text, True, BLACK)
        window.blit(button_text, (rect[0] + rect[2] // 2 - button_text.get_width() // 2,
                                  rect[1] + rect[3] // 2 - button_text.get_height() // 2))

# Function to draw status
def draw_status(battery, max_battery, steps, max_steps):
    if battery < 0 or steps > max_steps:
        return
    
    font = pygame.font.SysFont(None, 30)
    
    # Battery status on the left top corner
    battery_status_text = f"Battery: {battery}/{max_battery}"
    battery_status_surface = font.render(battery_status_text, True, BLACK)
    battery_x = margin
    battery_y = margin
    window.blit(battery_status_surface, (battery_x, battery_y))
    
    # Battery bar (battery icon style)
    bar_width = 200
    bar_height = 20
    battery_percentage = battery / max_battery
    battery_bar_length = int(bar_width * battery_percentage)
    
    battery_outer_rect = pygame.Rect(battery_x, battery_y + 30, bar_width + 10, bar_height + 10)
    battery_inner_rect = pygame.Rect(battery_x + 5, battery_y + 35, bar_width, bar_height)
    battery_fill_rect = pygame.Rect(battery_x + 5, battery_y + 35, battery_bar_length, bar_height)
    battery_tip_rect = pygame.Rect(battery_x + bar_width + 10, battery_y + 35 + bar_height // 4, 5, bar_height // 2)
    
    pygame.draw.rect(window, BLACK, battery_outer_rect, 2)  # Outer border
    pygame.draw.rect(window, BLACK, battery_tip_rect)  # Battery tip
    pygame.draw.rect(window, BatteryBar, battery_fill_rect)  # Battery fill
    pygame.draw.rect(window, BLACK, battery_inner_rect, 2)  # Inner border
    
    # Steps status on the right top corner
    steps_status_text = f"Steps: {steps}/{max_steps}"
    steps_status_surface = font.render(steps_status_text, True, BLACK)
    steps_x = screen_width - margin - steps_status_surface.get_width()
    steps_y = margin
    window.blit(steps_status_surface, (steps_x, steps_y))

    # Steps bar (gradient fill style)
    steps_bar_length = bar_width
    steps_bar_height = 20
    steps_bar_x = steps_x - bar_width + steps_status_surface.get_width()
    steps_bar_y = steps_y + 30

    steps_percentage = steps / max_steps
    steps_fill_length = int(steps_bar_length * steps_percentage)

    # Draw the empty steps bar
    pygame.draw.rect(window, BLACK, (steps_bar_x, steps_bar_y, steps_bar_length, steps_bar_height), 2)

    # Draw the filled part of the steps bar with gradient
    for i in range(steps_fill_length):
        gradient_color = (
            255,  # Red remains constant
            int(0 + 255 * (1 - i / steps_fill_length)),  # Green increases from 0 to 255
            int(255 * (1 - i / steps_fill_length))  # Blue decreases from 255 to 0
        )

        pygame.draw.rect(window, gradient_color, (steps_bar_x + i, steps_bar_y, 1, steps_bar_height))


# Run the simulation
running = True
paused = False
step_index = 0
clock = pygame.time.Clock()

battery_state = maxBattery

prev_x, prev_y = None, None

angle = 0

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            mouse_pos = pygame.mouse.get_pos()
            if screen_width // 2 - 100 <= mouse_pos[0] <= screen_width // 2 - 20 and screen_height + 40 <= mouse_pos[1] <= screen_height + 80:
                paused = False  # Resume button clicked
            elif screen_width // 2 + 20 <= mouse_pos[0] <= screen_width // 2 + 100 and screen_height + 40 <= mouse_pos[1] <= screen_height + 80:
                paused = True  # Pause button clicked
    
    window.fill(LIGHTYELLOW)
    charging = False
    if step_index < len(steps_history):
        charging = (docking_station['x'] == steps_history[step_index]['x'] and
                    docking_station['y'] == steps_history[step_index]['y'] and
                    prev_x == docking_station['x'] and
                    prev_y == docking_station['y'] and
                    house_matrix[docking_station['x']][docking_station['y']] == 0)
    draw_grid(charging)
    draw_legend()
    draw_buttons()
    draw_status(battery_state, maxBattery, step_index, maxSteps)

    if not paused and step_index < len(steps_history):
        # Draw the vacuum cleaner
        x, y = steps_history[step_index]['x'], steps_history[step_index]['y']
        start_x = (screen_width - width) // 2
        start_y = (screen_height - height) // 2
        vacuum_center = (start_x + y * block_size + block_size // 2,
                         start_y + x * block_size + block_size // 2)
        pygame.draw.circle(window, GOLD, vacuum_center, block_size // 4)

        # Draw a line to indicate direction (rotation)
        line_length = block_size // 6
        end_x = vacuum_center[0] + line_length * math.cos(math.radians(angle))
        end_y = vacuum_center[1] + line_length * math.sin(math.radians(angle))
        pygame.draw.line(window, MAROON, vacuum_center, (end_x, end_y), 2)

        # The angle is used for the rotating movement for the vacuum
        angle += 10

        # Decrementing the dirt level when staying in the same place, since this means the vacuum is cleaning
        if ((prev_x == None and prev_y == None and house_matrix[x][y] > 0)
            or (prev_x == x and prev_y == y and house_matrix[x][y] > 0)):
            house_matrix[x][y] -= 1

        prev_x, prev_y = x, y
        step_index += 1

        if battery_state < maxBattery and charging:
            battery_state = min(maxBattery, battery_state + maxBattery / 20.0)
        else:
            if(battery_state>0):
                battery_state-=1
            else:
                battery_state=0

    # Drawing the vacuum cleaner at its last position when paused or finished
    if (paused or step_index >= len(steps_history)) and prev_x is not None and prev_y is not None:
        vacuum_center = (start_x + prev_y * block_size + block_size // 2,
                         start_y + prev_x * block_size + block_size // 2)
        pygame.draw.circle(window, GOLD, vacuum_center, block_size // 4)
        end_x = vacuum_center[0] + line_length * math.cos(math.radians(angle))
        end_y = vacuum_center[1] + line_length * math.sin(math.radians(angle))
        pygame.draw.line(window, MAROON, vacuum_center, (end_x, end_y), 2)

    # Displaying "End of Simulation" when done
    if step_index >= len(steps_history):
        font = pygame.font.SysFont(None, 48)
        end_text = font.render("End of Simulation", True, BLACK)
        window.blit(end_text, (screen_width // 2 - end_text.get_width() // 2, 20))

    pygame.display.flip()
    clock.tick(6)  # Incrementing the clock tick will make the vacuum faster

pygame.quit()
