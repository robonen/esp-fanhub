import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import tailwindcss from '@tailwindcss/vite'
import { createGzip } from 'node:zlib'
import { createReadStream, createWriteStream, readdirSync, statSync, unlinkSync } from 'node:fs'
import { join } from 'node:path'

function gzipPlugin(deleteOriginal = false) {
  return {
    name: 'vite-plugin-gzip',
    apply: 'build' as const,
    closeBundle() {
      const outDir = '../data'
      
      function compressFile(filePath: string) {
        const gzip = createGzip({ level: 9 })
        const input = createReadStream(filePath)
        const output = createWriteStream(`${filePath}.gz`)
        
        output.on('finish', () => {
          console.log(`Compressed: ${filePath}.gz`)
          if (deleteOriginal) {
            unlinkSync(filePath)
            console.log(`Deleted: ${filePath}`)
          }
        })
        
        input.pipe(gzip).pipe(output)
      }
      
      function walkDir(dir: string) {
        const files = readdirSync(dir)
        
        files.forEach(file => {
          const filePath = join(dir, file)
          const stat = statSync(filePath)
          
          if (stat.isDirectory()) {
            walkDir(filePath)
          } else if (/\.(html|js|css)$/.test(file)) {
            compressFile(filePath)
          }
        })
      }
      
      walkDir(outDir)
    }
  }
}

export default defineConfig({
  plugins: [vue(), tailwindcss(), gzipPlugin(true)],
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
