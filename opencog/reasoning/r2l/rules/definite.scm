(define definite
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$noun")
                (TypeNode "WordInstanceNode")
            )
        )
        (ImplicationLink
            (AndLink
                (WordInstanceLink
                    (VariableNode "$noun")
                    (VariableNode "$a-parse")
                )
                (InheritanceLink
   			(VariableNode "$noun")
   			(DefinedLinguisticConceptNode "definite")
		)
            )
            (ExecutionOutputLink
          	  (GroundedSchemaNode "scm: pre-definite-rule")
           	      (ListLink
           	         (VariableNode "$noun")
			)
		)
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "definite-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "definite-Rule") definite)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-definite-rule noun)
    (definite-rule (word-inst-get-word-str noun)(cog-name noun)
    )
)


