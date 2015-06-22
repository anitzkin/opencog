
(define conj_but
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$word1")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$word2")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$pos")
                (TypeNode "DefinedLinguisticConceptNode")
            )
        )
        (butLink
            (WordInstanceLink
                (VariableNode "$word1")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$word2")
                (VariableNode "$a-parse")
            )
            (PartOfSpeechLink
   		(VariableNode "$word1")
   		(VariableNode "$pos")
	    )
	    (EvaluationLink
   		(PrepositionalRelationshipNode "conj_but")
   		(ListLink
      			(VariableNode "$word1")
      			(VariableNode "$word2")
   		)
	    )
        )
        (ExecutionOutputLink
       	   (GroundedSchemaNode "scm: pre-conj_but-rule")
       	      (ListLink
       	         (VariableNode "$word1")
		 (VariableNode "$word2")
       	         (VariableNode "$pos")
            )
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "conj_but-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "conj_but-Rule") conj_but)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-conj_but-rule word1 word2 pos)
    	(but-rule (word-inst-get-word-str word1) (cog-name word1)
	(but-rule (word-inst-get-word-str word2) (cog-name word2)
	(word-inst-get-word-str pos) (cog-name pos)
              
    )
)


