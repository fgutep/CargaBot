#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from lab1_msgs.msg import RobotAction
from lab1_msgs.srv import PlayRecording
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog
import threading
from datetime import datetime
import os

class TurtleBotInterfaceNode(Node):
    def __init__(self):
        super().__init__("turtle_bot_interface")
        
        # Suscriptor al tópico de posición (tipo Twist)
        self.position_subscription = self.create_subscription(
            Twist,
            '/turtlebot_position',
            self.position_callback,
            10
        )
        
        # Suscriptor al tópico de acciones (usando mensaje personalizado)
        self.action_subscription = self.create_subscription(
            RobotAction,
            '/turtlebot_actions',
            self.action_callback,
            10
        )
        
        # Listas para almacenar la trayectoria
        self.x_data = []
        self.y_data = []
        
        # Lista para almacenar las acciones
        self.actions_log = []
        
        # Bandera para controlar si hay nuevos datos
        self.new_data = False
        
        # Configuración de guardado
        self.save_actions = False
        self.actions_filename = None
        
        self.get_logger().info("TurtleBot Interface Node has been started.")
        self.get_logger().info("Listening to /turtlebot_position and /turtlebot_actions topics...")
    
    def position_callback(self, msg):
        """Callback que recibe la posición del robot (Twist)"""
        x = msg.linear.x
        y = msg.linear.y
        
        self.x_data.append(x)
        self.y_data.append(y)
        self.new_data = True
        
        self.get_logger().info(f"Position: x={x:.3f}m, y={y:.3f}m")
    
    def action_callback(self, msg):
        """Callback que recibe las acciones del teleop (RobotAction)"""
        if self.save_actions:
            # Extraer la tecla y descripción del campo action
            parts = msg.action.split('|')
            if len(parts) == 2:
                key, description = parts
                action_str = f"{msg.timestamp:.3f}|{key}|{description}"
                self.actions_log.append(action_str)
                self.get_logger().info(f"Action logged: {action_str}")

class TurtleBotGUI:
    def __init__(self, node):
        self.node = node
        
        # Crear ventana principal
        self.root = tk.Tk()
        self.root.title("TurtleBot Position Tracker")
        self.root.geometry("900x750")
        
        # Preguntar si desea guardar el recorrido
        self.ask_save_preference()
        
        # Frame superior para controles
        control_frame = tk.Frame(self.root)
        control_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)
        
        # Entrada para el nombre de la gráfica
        tk.Label(control_frame, text="Nombre de la gráfica:", font=("Arial", 10)).pack(side=tk.LEFT, padx=5)
        self.graph_name_entry = tk.Entry(control_frame, width=30, font=("Arial", 10))
        self.graph_name_entry.insert(0, "trayectoria_turtlebot")
        self.graph_name_entry.pack(side=tk.LEFT, padx=5)
        
        # Botón para guardar gráfica
        self.save_button = tk.Button(
            control_frame, 
            text="Guardar Gráfica", 
            command=self.save_graph,
            bg="#4CAF50",
            fg="white",
            font=("Arial", 10, "bold"),
            padx=10,
            pady=5
        )
        self.save_button.pack(side=tk.LEFT, padx=10)
        
        # Botón para guardar acciones manualmente
        if self.node.save_actions:
            self.save_actions_button = tk.Button(
                control_frame,
                text="Guardar Acciones",
                command=self.save_actions_manually,
                bg="#2196F3",
                fg="white",
                font=("Arial", 10, "bold"),
                padx=10,
                pady=5
            )
            self.save_actions_button.pack(side=tk.LEFT, padx=5)
        
        # Botón para reproducir grabación
        self.play_button = tk.Button(
            control_frame,
            text="Reproducir Grabación",
            command=self.play_recording,
            bg="#9C27B0",
            fg="white",
            font=("Arial", 10, "bold"),
            padx=10,
            pady=5
        )
        self.play_button.pack(side=tk.LEFT, padx=5)
        
        # Botón para limpiar trayectoria
        self.clear_button = tk.Button(
            control_frame,
            text="Limpiar Trayectoria",
            command=self.clear_trajectory,
            bg="#f44336",
            fg="white",
            font=("Arial", 10, "bold"),
            padx=10,
            pady=5
        )
        self.clear_button.pack(side=tk.LEFT, padx=5)
        
        # Label para mostrar información
        info_text = "Esperando datos del robot..."
        if self.node.save_actions:
            info_text += f" | Guardando acciones en: {self.node.actions_filename}"
        
        self.info_label = tk.Label(
            self.root, 
            text=info_text, 
            font=("Arial", 9),
            fg="blue",
            wraplength=850
        )
        self.info_label.pack(pady=5)
        
        # Label para contador de acciones
        self.actions_label = tk.Label(
            self.root,
            text="Acciones registradas: 0",
            font=("Arial", 9),
            fg="green"
        )
        if self.node.save_actions:
            self.actions_label.pack(pady=2)
        
        # Crear figura de matplotlib
        self.fig, self.ax = plt.subplots(figsize=(8, 5.5))
        self.ax.set_xlabel('X (metros)', fontsize=12)
        self.ax.set_ylabel('Y (metros)', fontsize=12)
        self.ax.set_title('Trayectoria del TurtleBot en Tiempo Real', fontsize=14, fontweight='bold')
        self.ax.grid(True, linestyle='--', alpha=0.7)
        self.ax.set_aspect('equal', adjustable='box')
        
        # Línea de la trayectoria
        self.line, = self.ax.plot([], [], 'b-', linewidth=2, label='Trayectoria')
        # Punto actual del robot
        self.current_point, = self.ax.plot([], [], 'ro', markersize=10, label='Posición actual')
        # Punto inicial
        self.start_point, = self.ax.plot([], [], 'go', markersize=12, label='Inicio')
        
        self.ax.legend(loc='upper right')
        
        # Integrar matplotlib en tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Configurar el cierre de la ventana
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
    
    def ask_save_preference(self):
        """Pregunta al usuario si desea guardar las acciones"""
        response = messagebox.askyesno(
            "Guardar Recorrido",
            "¿Desea guardar la secuencia de acciones del robot durante el recorrido?"
        )
        
        if response:
            # Pedir nombre del archivo
            filename = simpledialog.askstring(
                "Nombre del Archivo",
                "Ingrese el nombre para el archivo de acciones (sin extensión):",
                initialvalue=f"acciones_robot_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
            )
            
            if filename:
                # Asegurar que termine en .txt
                if not filename.endswith('.txt'):
                    filename += '.txt'
                
                self.node.save_actions = True
                self.node.actions_filename = filename
                self.node.get_logger().info(f"Actions will be saved to: {filename}")
            else:
                self.node.save_actions = False
                messagebox.showinfo("Información", "No se guardarán las acciones.")
        else:
            self.node.save_actions = False
            self.node.get_logger().info("Actions will not be saved.")
    
    def play_recording(self):
        """Llama al servicio para reproducir una grabación"""
        # Abrir diálogo para seleccionar archivo
        file_path = filedialog.askopenfilename(
            title="Seleccionar archivo de grabación",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")],
            initialdir=os.path.expanduser("~")
        )
        
        if not file_path:
            return
        
        # Crear cliente del servicio
        client = self.node.create_client(PlayRecording, 'play_recording')
        
        if not client.wait_for_service(timeout_sec=5.0):
            messagebox.showerror(
                "Error", 
                "El servicio 'play_recording' no está disponible.\n\n"
                "Asegúrese de que el nodo 'turtle_bot_player' está ejecutándose:\n"
                "ros2 run lab1_py_pkg turtle_bot_player"
            )
            return
        
        # Crear request
        request = PlayRecording.Request()
        request.filename = file_path
        
        self.node.get_logger().info(f"Calling service to play: {file_path}")
        
        # Deshabilitar botón durante reproducción
        self.play_button.config(state='disabled', text="Reproduciendo...")
        
        # Mostrar mensaje de "cargando"
        filename_display = os.path.basename(file_path)
        self.info_label.config(text=f"Reproduciendo: {filename_display}...", fg="orange")
        
        # Llamar al servicio de forma asíncrona
        future = client.call_async(request)
        
        # Callback cuando el servicio responde
        def response_callback(future):
            try:
                response = future.result()
                
                # Re-habilitar botón
                self.play_button.config(state='normal', text="Reproducir Grabación")
                
                if response.success:
                    messagebox.showinfo("Éxito", response.message)
                    self.node.get_logger().info(f"Playback successful: {response.message}")
                    self.info_label.config(text=f"Reproducción completada: {filename_display}", fg="green")
                else:
                    messagebox.showerror("Error", response.message)
                    self.node.get_logger().error(f"Playback failed: {response.message}")
                    self.info_label.config(text="Error en la reproducción", fg="red")
                    
            except Exception as e:
                self.play_button.config(state='normal', text="Reproducir Grabación")
                messagebox.showerror("Error", f"Error llamando al servicio:\n{str(e)}")
                self.node.get_logger().error(f"Service call exception: {str(e)}")
                self.info_label.config(text="Error al llamar al servicio", fg="red")
        
        future.add_done_callback(response_callback)
    
    def update_plot(self):
        """Actualiza la gráfica con nuevos datos"""
        if self.node.new_data and len(self.node.x_data) > 0:
            # Actualizar trayectoria
            self.line.set_data(self.node.x_data, self.node.y_data)
            
            # Actualizar punto actual
            self.current_point.set_data([self.node.x_data[-1]], [self.node.y_data[-1]])
            
            # Mostrar punto inicial
            if len(self.node.x_data) > 0:
                self.start_point.set_data([self.node.x_data[0]], [self.node.y_data[0]])
            
            # Ajustar límites de los ejes automáticamente
            if len(self.node.x_data) > 1:
                x_margin = (max(self.node.x_data) - min(self.node.x_data)) * 0.1 + 0.5
                y_margin = (max(self.node.y_data) - min(self.node.y_data)) * 0.1 + 0.5
                
                self.ax.set_xlim(min(self.node.x_data) - x_margin, max(self.node.x_data) + x_margin)
                self.ax.set_ylim(min(self.node.y_data) - y_margin, max(self.node.y_data) + y_margin)
            
            # Actualizar label de información
            num_points = len(self.node.x_data)
            current_x = self.node.x_data[-1]
            current_y = self.node.y_data[-1]
            
            info_text = f"Puntos registrados: {num_points} | Posición actual: X={current_x:.3f}m, Y={current_y:.3f}m"
            if self.node.save_actions:
                info_text += f" | Archivo: {self.node.actions_filename}"
            
            self.info_label.config(text=info_text, fg="green")
            
            self.canvas.draw()
            self.node.new_data = False
        
        # Actualizar contador de acciones
        if self.node.save_actions:
            self.actions_label.config(text=f"Acciones registradas: {len(self.node.actions_log)}")
        
        # Programar próxima actualización
        self.root.after(100, self.update_plot)
    
    def save_graph(self):
        """Guarda la gráfica en un archivo"""
        if len(self.node.x_data) == 0:
            messagebox.showwarning("Advertencia", "No hay datos para guardar.")
            return
        
        graph_name = self.graph_name_entry.get()
        if not graph_name:
            messagebox.showerror("Error", "Por favor ingrese un nombre para la gráfica.")
            return
        
        file_path = filedialog.asksaveasfilename(
            defaultextension=".png",
            filetypes=[
                ("PNG files", "*.png"),
                ("PDF files", "*.pdf"),
                ("SVG files", "*.svg"),
                ("All files", "*.*")
            ],
            initialfile=graph_name
        )
        
        if file_path:
            try:
                self.fig.savefig(file_path, dpi=300, bbox_inches='tight')
                messagebox.showinfo("Éxito", f"Gráfica guardada exitosamente en:\n{file_path}")
                self.node.get_logger().info(f"Graph saved: {file_path}")
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo guardar la gráfica:\n{str(e)}")
                self.node.get_logger().error(f"Error saving graph: {str(e)}")
    
    def save_actions_manually(self):
        """Guarda las acciones manualmente en el archivo"""
        if not self.node.save_actions:
            messagebox.showwarning("Advertencia", "El guardado de acciones no está habilitado.")
            return
        
        if len(self.node.actions_log) == 0:
            messagebox.showwarning("Advertencia", "No hay acciones para guardar.")
            return
        
        file_path = filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")],
            initialfile=self.node.actions_filename
        )
        
        if file_path:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write("=" * 60 + "\n")
                    f.write("REGISTRO DE ACCIONES DEL TURTLEBOT\n")
                    f.write("=" * 60 + "\n")
                    f.write(f"Fecha: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
                    f.write(f"Total de acciones: {len(self.node.actions_log)}\n")
                    f.write("=" * 60 + "\n\n")
                    f.write("Formato: Timestamp | Tecla | Acción\n")
                    f.write("-" * 60 + "\n")
                    
                    for i, action in enumerate(self.node.actions_log, 1):
                        parts = action.split('|')
                        if len(parts) == 3:
                            timestamp, key, description = parts
                            f.write(f"{i:4d}. {timestamp:>12s} | {key:^5s} | {description}\n")
                    
                    f.write("=" * 60 + "\n")
                
                messagebox.showinfo("Éxito", f"Acciones guardadas exitosamente en:\n{file_path}")
                self.node.get_logger().info(f"Actions saved: {file_path}")
            except Exception as e:
                messagebox.showerror("Error", f"No se pudieron guardar las acciones:\n{str(e)}")
                self.node.get_logger().error(f"Error saving actions: {str(e)}")
    
    def clear_trajectory(self):
        """Limpia la trayectoria guardada"""
        if messagebox.askyesno("Confirmar", "¿Está seguro de que desea limpiar la trayectoria y las acciones?"):
            self.node.x_data.clear()
            self.node.y_data.clear()
            self.node.actions_log.clear()
            self.line.set_data([], [])
            self.current_point.set_data([], [])
            self.start_point.set_data([], [])
            self.canvas.draw()
            self.info_label.config(text="Trayectoria y acciones limpiadas. Esperando nuevos datos...", fg="blue")
            if self.node.save_actions:
                self.actions_label.config(text="Acciones registradas: 0")
            self.node.get_logger().info("Trajectory and actions cleared")
    
    def on_closing(self):
        """Maneja el cierre de la ventana"""
        # Si hay acciones guardadas, preguntar si desea guardarlas
        if self.node.save_actions and len(self.node.actions_log) > 0:
            save_before_exit = messagebox.askyesnocancel(
                "Guardar antes de salir",
                f"Hay {len(self.node.actions_log)} acciones registradas.\n¿Desea guardarlas antes de salir?"
            )
            
            if save_before_exit is None:  # Cancelar
                return
            elif save_before_exit:  # Sí
                self.save_actions_manually()
        
        if messagebox.askokcancel("Salir", "¿Desea cerrar la interfaz?"):
            self.root.quit()
            self.root.destroy()
    
    def run(self):
        """Inicia la interfaz gráfica"""
        self.update_plot()
        self.root.mainloop()

def spin_node(node):
    """Función para ejecutar el spin de ROS en un thread separado"""
    rclpy.spin(node)

def main(args=None):
    rclpy.init(args=args)
    
    node = TurtleBotInterfaceNode()
    
    ros_thread = threading.Thread(target=spin_node, args=(node,), daemon=True)
    ros_thread.start()
    
    gui = TurtleBotGUI(node)
    
    try:
        gui.run()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()