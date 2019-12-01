const path = require('path');

const Dotenv = require('dotenv-webpack')
const CopyPlugin = require('copy-webpack-plugin');

const htmlCopy = [
    'playground',
];

const folderCopy = [
    './bin'
];

const staticPath = path.join(__dirname, 'static');
const htmlPath = path.join(__dirname, 'html');

function getCopyPaths() {
    return htmlCopy.
        map(url => ({
            from: path.join(htmlPath, `${url}.html`),
            to: staticPath
        })).
        concat(folderCopy.map(p => ({
            from: path.join(__dirname, p),
            to: staticPath,
        })));
}

module.exports = {
    entry: {
        playground: './source/js/index.ts',
    },

    plugins: [
        new CopyPlugin(getCopyPaths()),
        new Dotenv(),
    ],

    module: {
        rules: [{
            test: /.tsx|ts$/,
            exclude: /node_modules/,
            use: {
                loader: 'ts-loader'
            }
        }]
    },

    resolve: {
      extensions: ['.tsx', '.ts', '.js'],
    },

    optimization: {
		// We no not want to minimize our code.
		minimize: false
	},

    output: {
        filename: '[name].js',
        path: path.resolve(__dirname, 'static')
    }
};

