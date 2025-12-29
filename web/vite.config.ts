import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import tailwindcss from '@tailwindcss/vite'

export default defineConfig({
  plugins: [vue(), tailwindcss()],
  define: {
    __VUE_OPTIONS_API__: false,
  },
  build: {
    outDir: '../data',
    emptyOutDir: true,
  },
  esbuild: {
    legalComments: 'none',
  },
  server: {
    port: 5173
  }
})
