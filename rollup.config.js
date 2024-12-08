import { resolve } from "path";
import { defineConfig } from "rollup";
import terser from "@rollup/plugin-terser";
import typescript from "@rollup/plugin-typescript";
import babel from "@rollup/plugin-babel";
import commonjs from "@rollup/plugin-commonjs";
import json from "@rollup/plugin-json";
import nodeResolve from "@rollup/plugin-node-resolve";
import nodeExternals from "rollup-plugin-node-externals";

const mode = process.env.NODE_ENV ?? "dev";

const shouldMinify = () => mode === "prod";

function overlay(path, src) {
    path = resolve(path);

    return { load: id => path === id ? src : null };
}

export default defineConfig({
    input: "./src/main.ts",
    plugins: [
        typescript({
            compilerOptions: {
                allowImportingTsExtensions: true,
                noEmit: true,
            },
            noForceEmit: true,
            noEmitOnError: true,
        }),
        babel({
            extensions: [".js", ".ts"],
            exclude: "node_modules/**",
            babelHelpers: "bundled",
            plugins: [
                ["@babel/plugin-transform-typescript", {
                    onlyRemoveTypeImports: true,
                    optimizeConstEnums: true,
                }],
                ["@babel/plugin-proposal-decorators", { legacy: true }],
                ["@babel/plugin-transform-class-properties"],
            ]
        }),
        nodeResolve(),
        commonjs(),
        json(),
        nodeExternals(),
        shouldMinify() && terser({ sourceMap: true })
    ],
    output: {
        dir: "dst",
        format: "esm",
        sourcemap: true,
        inlineDynamicImports: true,
    },
});
