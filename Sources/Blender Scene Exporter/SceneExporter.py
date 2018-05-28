import bpy
from bpy.types import Panel

class ExporterPanel(Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'
    bl_label = 'Tools Tab Label'
    bl_context = 'objectname'
    bl_category = 'SceneExporter'
    
    def draw(self, context):
        layout = self.layout
        layout.operator('primitive_cube_add', text = 'Add new cube.')
        
# Register call
def register():
    bpy.utils.register_class(ExporterPanel)
    
# Unregister call
def unregister():
    bpy.utils.unregister_class(ExporterPanel)
    
if (__name__ == '__main__'):
    register()