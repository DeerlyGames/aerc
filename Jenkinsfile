pipeline {
    agent none
    stages {
        stage("Generating Environments") {
            parallel {
                stage("MacOS"){
                    agent {
                        label "MacMini"
                    }
                    steps {
                        sh "chmod +x aemake && aemake gmake"
                    }
                }
            }
        }
        stage("Compiling") {
            parallel {
                stage("MacOS"){
                    agent {
                        label "MacMini"
                    }
                    steps {
                        sh "make"
                    }
                }
            }
        }
    }
}

